/********************************************************************************
 *  File Name       : main.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-02 15:19
 *  Description     : Main process of the Controled Multisense Word Embedding toolkit
********************************************************************************/

#include <pthread.h>
#include "common.h"
#include "vocab.h"
#include "embed.h"
#include "ds.h"
#include "pseudo.h"
#include "test.h"

/* libraries for parsing xml file */
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>

/* default file path */
#define DEFAULT_PARA_LIST_PATH "./parameters.xml"
#define DEFAULT_VOCAB_PATH "./vocab.txt"
#define DEFAULT_CORPUS_PATH "./corpus.txt"
#define DEFAULT_VECTOR_PATH "./vector.txt"

/* global variables */

/* vector parameters */
int dim = 50;                       /* vector space dimension */
int sense_num = 3;                  /* sense number in multisense embedding and 
                                       maxinum sense number in non-parametric multisense embedding */
/* embedding parameters */
int context_size = 5;               /* half size of context window */
double base_embed_alpha = 0.05;     /* base learning rate */
double embed_alpha;
int negative = 5;                   /* number of negative samples */
double subsample = 0.25;            /* subsampling rate */
int sense_model = MULTI_SENSE;      /* sense model - SINGLE_SENSE, MULTI_SENSE, NP_MULTI_SENSE */

/* helper parameters */
unsigned long long* random_num;     /* pseudo random number for each thread */
unsigned long long next_random = 1;

int total_senses = 0;

/* local variables */

/* vocabulary parameters */
static int learn_vocab = 0;
static int load_vocab = 0;
static int min_word_count = 15;

/* file path */
static char para_list_path[MAX_STRING];
static char vocab_corpus_path[MAX_STRING];
static char save_vocab_path[MAX_STRING];
static char load_vocab_path[MAX_STRING];
static char corpus_path[MAX_STRING];
static char vector_path[MAX_STRING];

/* thread parameters */
static int thread_num = 20;
static pthread_mutex_t mutex;

/* word count */
static long long word_count_actual = 0;
static long long corpus_words = 0;
static long long corpus_size = 0;

static int load_vec = 0;            /* learn word vector or just load word vector */

static double pseudo_alpha = 0.005;

/* tests */
static int test_wordsim353 = 0;
static int test_scws = 0;
static int test_analogy = 0;

/* local functions */
static void InitFilePath();
static void GetParameters(xmlDocPtr, xmlNodePtr);
static void Parse(char*);
static void* WordEmbedding(void*);
static void GetAllSenses();

/*
 * InitFilePath - set default file path
 */
static void InitFilePath() {
    strcpy(para_list_path, DEFAULT_PARA_LIST_PATH);
    strcpy(vocab_corpus_path, DEFAULT_CORPUS_PATH);
    strcpy(save_vocab_path, DEFAULT_VOCAB_PATH);
    strcpy(load_vocab_path, DEFAULT_VOCAB_PATH);
    strcpy(corpus_path, DEFAULT_CORPUS_PATH);
    strcpy(vector_path, DEFAULT_VECTOR_PATH);
}

static void GetParameters(xmlDocPtr doc, xmlNodePtr cur) {
    char* parameter_name;
    char* parameter_value;
    for (cur = cur->xmlChildrenNode; cur != NULL; cur = cur->next) {
        if (xmlStrcmp(cur->name, (const xmlChar*)"parameter") != 0) continue;
        parameter_name = (char*)xmlGetProp(cur, BAD_CAST("name"));
        parameter_value = (char*)xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);


        /* vocabulary parameters */
        if (!strcmp(parameter_name, "learn vocab")) {
            learn_vocab = 1;
            if (parameter_value != NULL)
                strcpy(vocab_corpus_path, parameter_value);
        }
        if (!strcmp(parameter_name, "save vocab")) {
            if (parameter_value != NULL) 
                strcpy(save_vocab_path, parameter_value);
        }
        if (!strcmp(parameter_name, "load vocab")) {
            load_vocab = 1;
            if (parameter_value != NULL)
                strcpy(load_vocab_path, parameter_value);
        }
        if (!strcmp(parameter_name, "min word count")) min_word_count = atoi(parameter_value);

        /* embedding parameters */
        if (!strcmp(parameter_name, "dimension")) dim = atoi(parameter_value);
        if (!strcmp(parameter_name, "context size")) context_size = atoi(parameter_value);
        if (!strcmp(parameter_name, "corpus")) strcpy(corpus_path, parameter_value);
        if (!strcmp(parameter_name, "embed alpha")) base_embed_alpha = atof(parameter_value);
        if (!strcmp(parameter_name, "subsample")) subsample = atof(parameter_value);
        if (!strcmp(parameter_name, "negative")) negative = atoi(parameter_value);
        if (!strcmp(parameter_name, "sense model")) {
            if (!strcmp(parameter_value, "single sense")) sense_model = SINGLE_SENSE;
            else if (!strcmp(parameter_value, "multi-sense")) sense_model = MULTI_SENSE;
            else if (!strcmp(parameter_value, "non-parametric multi-sense")) sense_model = NP_MULTI_SENSE;
        }
        if (!strcmp(parameter_name, "save vector")) strcpy(vector_path, parameter_value);
        if (!strcmp(parameter_name, "thread")) thread_num = atoi(parameter_value);
        if (!strcmp(parameter_name, "sense number")) sense_num = atoi(parameter_value);
        if (!strcmp(parameter_name, "load vector")) load_vec = 1;

        /* pseudo senses detection parameters */
        if (!strcmp(parameter_name, "evaluation")) {
            if (!strcmp(parameter_value, "unsupervised")) evaluation = UNSUPERVISED;
            if (!strcmp(parameter_value, "supervised")) evaluation = SUPERVISED;
        }
        if (!strcmp(parameter_name, "pseudo alpha")) pseudo_alpha = atof(parameter_value);

        /* tests parameters */
        if (!strcmp(parameter_name, "wordsim353")) test_wordsim353 = 1;
        if (!strcmp(parameter_name, "scws")) test_scws = 1;
        if (!strcmp(parameter_name, "analogy")) test_analogy = 1;
    }
}

/* 
 * Parse - parse parameters file
 */
static void Parse(char* file_path) {
    xmlDocPtr doc = xmlParseFile(file_path);
    if (doc == NULL) 
        ReportError("Failed to get parameters");
    
    xmlNodePtr cur = xmlDocGetRootElement(doc);
    if (cur == NULL) 
        ReportError("Failed to get parameters");

    for (cur = cur->xmlChildrenNode; cur != NULL; cur = cur->next) {
        if (xmlStrcmp(cur->name, (const xmlChar*)"parameter_list")) continue;
        GetParameters(doc, cur);
    }
    xmlFreeDoc(doc);
}

static void* WordEmbedding(void* id) {
    long long tid = (long long)id;
    random_num[tid] = (unsigned long long)id;
    /* open corpus and seek start position of this pthread */
    FILE* corpus = fopen(corpus_path, "r");
    fseek(corpus, corpus_size / (long long)thread_num * (long long)id, SEEK_SET);

    /* count trained word */
    long long word_count = 0;
    long long last_word_count = 0;

    int sentence_length = 0; // sentense length
    int sen[MAX_SENTENCE_LENGTH + 1];
    
    while (1) {
        if (word_count - last_word_count > 10000) {
            pthread_mutex_lock(&mutex);
            word_count_actual += word_count - last_word_count;
            last_word_count = word_count;
            embed_alpha = base_embed_alpha * (1 - word_count_actual / (double)(corpus_words + 1));
            if (embed_alpha < 0.0001 * base_embed_alpha) embed_alpha = base_embed_alpha * 0.0001;
            fprintf(stdout, "Word embedding process: %.3f%%  Alpha: %.6f%c", (double)word_count_actual / corpus_words * 100, embed_alpha, 13);
            fflush(stdout);
            pthread_mutex_unlock(&mutex);
        }
        
        sentence_length = ReadSentence(corpus, sen, corpus_words, subsample, &random_num[tid]);
        word_count += sentence_length;

        if (sense_model == SINGLE_SENSE) SingleSense(tid, sen, sentence_length);
        else if (sense_model == MULTI_SENSE) MultiSense(tid, sen, sentence_length);
        else if (sense_model == NP_MULTI_SENSE) NonParametricMultiSense(tid, sen, sentence_length);

        /* end this thread */
        if (feof(corpus) || word_count > corpus_words / thread_num) {
            //pthread_mutex_lock(&mutex);
            word_count_actual += word_count - last_word_count;
            //pthread_mutex_unlock(&mutex);
            break;
        }
    }

    pthread_exit(NULL);
}

static void GetAllSenses() {
    int i, k = 0, word_num = GetWordNum();
    for (i = 0; i < word_num; i++)
        total_senses += word_vec[i].sense_num;
    tree_node = (double**)malloc(total_senses * sizeof(double*));
    for (i = 0; i < word_num; i++) {
        vec_list_t* sense = word_vec[i].sense;
        for (; sense != NULL; sense = sense->next, k++)
            tree_node[k] = sense->sense_vec;
    }
}

int main(int argc, char** argv) {
    /* get parameters */
    InitFilePath();
    if (argc == 2) strcpy(para_list_path, argv[1]);
    Parse(para_list_path);

    /* load vector and load vocabulary meanwhile */
    if (load_vec) {
        fprintf(stdout, "Start loading vectors...\n");
        corpus_words = LoadVocab(load_vocab_path, min_word_count);
        word_num = GetWordNum();
        LoadWordVec(vector_path);
        fprintf(stdout, "End loading vectors.\n");
    }
    /* build or load vocabulary and then train word vectors */
    else {
        /* build vocabulary */
        /* it is possible to load vocabulary and then expand it */
        fprintf(stdout, "Start building vocabulary...\n");
        if (load_vocab) corpus_words = LoadVocab(load_vocab_path, min_word_count);
        if (learn_vocab) corpus_words = LearnVocab(vocab_corpus_path, save_vocab_path, min_word_count);
        fprintf(stdout, "End building vocabulary.\n");

        /* initialize word embedding */
        fprintf(stdout, "Start word embedding...\n");
        word_num = GetWordNum();
        InitWordVec();
        if (negative == 0)
            InitHierarchical();
        else InitNegative();
        corpus_size = GetFileSize(corpus_path); /* get corpus size */

        /* concurrently word embedding */
        long long i;
        embed_alpha = base_embed_alpha;
        pthread_mutex_init(&mutex, NULL);
        pthread_t* tid = (pthread_t*)malloc(thread_num * sizeof(pthread_t));
        random_num = (unsigned long long*)malloc(thread_num * sizeof(unsigned long long));
        for (i = 0; i < thread_num; i++) pthread_create(&tid[i], NULL, WordEmbedding, (void*)i);
        for (i = 0; i < thread_num; i++) pthread_join(tid[i], NULL);
    
        SaveWordVec(vector_path);
        free(random_num);
        fprintf(stdout, "End word embedding.\n");
    }

    /* build kd-tree */
    GetAllSenses();
    BuildKDTree(0, total_senses - 1, 0, dim);

    fprintf(stdout, "Test before transform:\n");
    if (test_wordsim353) TestWordsim353();
    if (test_scws) TestSCWS();
    if (test_analogy) TestAnalogy();

    /* pseudo senses detection */
    DetectPseudoSenses(word_num);
    double* matrix = TrainMatrix(pseudo_pairs[0], pseudo_pairs[1], pseudo_pairs_num,
            10, pseudo_alpha, PseudoEliminationEuclid);
    PseudoTransform(matrix);

    fprintf(stdout, "Test after transform:\n");
    if (test_wordsim353) TestWordsim353();
    if (test_scws) TestSCWS();
    //if (test_analogy) TestAnalogy();
    //
    Train();
    return 0;
}

/********************************************************************************
 *  File Name       : main.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-02 15:19
 *  Description     : Main process of the Controled Multisense Word Embedding toolkit
********************************************************************************/

#include "cmg.h"

/* default file path */
#define DEFAULT_VOCAB_PATH "./vocab.txt"
#define DEFAULT_CORPUS_PATH "./corpus.txt"
#define DEFAULT_VECTOR_PATH "./vector.txt"

/* global parameter */
int dim = 50;
int window = 5;
int min_count = 15;
double start_alpha = 0.05;
double alpha;
int sense_num = 3;

unsigned long long* random_num;

int negative = 5;
static double subsampling = 0.0;

int model = SINGLE_SENSE;

/* file path */
static char corpus_path[MAX_STRING];
static char save_vocab_path[MAX_STRING];
static char load_vocab_path[MAX_STRING];
static char vector_path[MAX_STRING];

static int thread_num = 20;
static long long total_words = 0;
static long long corpus_size = 0;

static long long word_count_actual = 0;
static int load_vocab = 0;

static pthread_mutex_t mutex;

static int ArgPos(char*, int, char**);
static void Parse(int, char**);
static void GetCorpusSize();
static void* WordEmbedding(void*);

// copy from google word2vec tool
static int ArgPos(char* str, int argc, char** argv) {
    register int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(str, argv[i])) {
            if (i == argc - 1) {
                printf("Argument missing for %s\n", str);
                exit(1);
            }
            return i;
        }
    }
    return -1;
}

static void Parse(int argc, char** argv) {
    register int i;
    if ((i = ArgPos((char*)"-dim", argc, argv)) > 0) dim = atoi(argv[i + 1]);
    if ((i = ArgPos((char*)"-window", argc, argv)) > 0) window = atoi(argv[i + 1]);
    if ((i = ArgPos((char*)"-corpus", argc, argv)) > 0) strcpy(corpus_path, argv[i + 1]);
    if ((i = ArgPos((char*)"-min-count", argc, argv)) > 0) min_count = atoi(argv[i + 1]);
    if ((i = ArgPos((char*)"-alpha", argc, argv)) > 0) start_alpha = atof(argv[i + 1]);
    if ((i = ArgPos((char*)"-save-vocab", argc, argv)) > 0) strcpy(save_vocab_path, argv[i + 1]);
    if ((i = ArgPos((char*)"-load-vocab", argc, argv)) > 0) {
        strcpy(load_vocab_path, argv[i + 1]);
        load_vocab = 1;
    }
    if ((i = ArgPos((char*)"-subsampling", argc, argv)) > 0) subsampling = atof(argv[i + 1]);
    if ((i = ArgPos((char*)"-negative", argc, argv)) > 0) negative = atoi(argv[i + 1]);
    if ((i = ArgPos((char*)"-model", argc, argv)) > 0) model = atoi(argv[i + 1]);
    if ((i = ArgPos((char*)"-vector", argc, argv)) > 0) strcpy(vector_path, argv[i + 1]);
    if ((i = ArgPos((char*)"-thread", argc, argv)) > 0) thread_num = atoi(argv[i + 1]);
    if ((i = ArgPos((char*)"-sense-num", argc, argv)) > 0) sense_num = atoi(argv[i + 1]);
}

static void GetCorpusSize() {
    FILE* corpus = fopen(corpus_path, "rb");
    if (corpus == NULL)
        ReportError("Corpus is not existed!");
    fseek(corpus, 0, SEEK_END);
    corpus_size = ftell(corpus);
    fclose(corpus);
}

static void* WordEmbedding(void* id) {
    long long pid = (long long)id;
    random_num[pid] = (unsigned long long)id;
    /* open corpus and seek start position of this pthread */
    FILE* corpus = fopen(corpus_path, "rb");
    fseek(corpus, corpus_size / (long long)thread_num * (long long)id, SEEK_SET);

    /* count trained word */
    long long word_count = 0;
    long long last_word_count = 0;

    int sentence_length = 0; // sentense length
    int sen[MAX_SENTENCE_LENGTH + 1];
    int word;
    
    while (1) {
        if (word_count - last_word_count > 10000) {
            pthread_mutex_lock(&mutex);
            word_count_actual += word_count - last_word_count;
            last_word_count = word_count;
             alpha = start_alpha * (1 - word_count_actual / (double)(total_words + 1));
            pthread_mutex_unlock(&mutex);
        }
        
        sentence_length = 0;
        /* read a sentence */
        while (1) {
            /* read a word */
            word = ReadWordIndex(corpus);
            if (feof(corpus)) break;
            if (word == NOT_IN_VOCAB) continue;
            if (word == END_OF_SENTENCE) break;
            word_count++;

            /* subsampling */
            if (subsampling > 0) {
                random_num[pid] = NextRandomNum(random_num[pid]);
                int word_cnt = GetWordCountByIdx(word);
                double prob = (sqrt(word_cnt / (subsampling * total_words)) + 1) 
                    * (subsampling * total_words) / word_cnt;
                if (prob < RandomRealNum(random_num[pid])) continue;
            }

            sen[sentence_length] = word;
            sentence_length++;
            if (sentence_length >= MAX_SENTENCE_LENGTH) break;
        }

        if (model == SINGLE_SENSE) SingleSense(pid, sen, sentence_length);
        else if (model == MULTI_SENSE) MultiSense(pid, sen, sentence_length);
        else if (model == NP_MULTI_SENSE) NonParametricMultiSense(pid, sen, sentence_length);

        /* end this thread */
        if (feof(corpus) || word_count > total_words / thread_num) {
            pthread_mutex_lock(&mutex);
            word_count_actual += word_count - last_word_count;
            pthread_mutex_unlock(&mutex);
            break;
        }
    }

    pthread_exit(NULL);
}

int main(int argc, char** argv) {
    /* initialize file path */
    strcpy(corpus_path, DEFAULT_CORPUS_PATH);
    strcpy(save_vocab_path, DEFAULT_VOCAB_PATH);
    strcpy(load_vocab_path, DEFAULT_VOCAB_PATH);
    strcpy(vector_path, DEFAULT_VECTOR_PATH);

    /* parse command line instructors */
    Parse(argc, argv);

    /* build vocabulary */
    if (load_vocab) total_words = LoadVocab(load_vocab_path, min_count);
    else total_words = LearnVocab(corpus_path, save_vocab_path, min_count);

    printf("vocabulary is built!\n");

    /* initialize word embedding */
    InitWordVec(dim);
    if (negative > 0)
        InitNegative(dim);
    GetCorpusSize();
    
    /* concurrently word embedding */
    long long i;
    pthread_mutex_init(&mutex, NULL);
    pthread_t* pid = (pthread_t*)malloc(thread_num * sizeof(pthread_t));
    random_num = (unsigned long long*)malloc(thread_num * sizeof(unsigned long long));
    for (i = 0; i < thread_num; i++) pthread_create(&pid[i], NULL, WordEmbedding, (void*)i);
    for (i = 0; i < thread_num; i++) pthread_join(pid[i], NULL);
    
    SaveWordVec(vector_path);
    return 0;
}

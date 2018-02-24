/********************************************************************************
 *  File Name       : vocab.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-02 16:54
 *  Description     : Vocabulary
********************************************************************************/

#include "common.h"
#include "vocab.h"

/* local vocabulary variables */
static vocab_word_t* vocab = NULL;
static int vocab_size = 0;

/* local vocabulary functions */
static int WordCntCmp(const void*, const void*);
static void InitVocab();
static int AddToVocab(char*);
static long long SortVocab(int);

/* 
 * WordCntCmp - compare funtion for word count
 */
static int WordCntCmp(const void* a, const void* b) {
    return ((vocab_word_t*)b)->cnt - ((vocab_word_t*)a)->cnt;
}

/* 
 * InitVocab - initialize hash table and then alloc storage for vocabulary
 */
static void InitVocab() {
    if (vocab == NULL) {
        InitHashTable();
        vocab = (vocab_word_t*)malloc(MAX_VOCAB_SIZE * sizeof(vocab_word_t));
    }
}
/*
 * AddToVocab - add a word into vocabulary
 */
static int AddToVocab(char* word) {
    /* copy the word to vocabulary */
    int len = strlen(word) + 1;
    vocab[vocab_size].word = (char*)calloc(len, sizeof(char));
    strcpy(vocab[vocab_size].word, word);
    vocab[vocab_size].cnt = 0;

    /* update vocabulary size */
    vocab_size++;
    return vocab_size - 1;
}

/* 
 * SortVocab - sort vocabulary depend on word count
 *             then return total words in corpus
 */
static long long SortVocab(int min_count) {
    // sort vocabulary
    qsort(vocab, vocab_size, sizeof(vocab_word_t), WordCntCmp);

    // rebuild hash table
    long long i, total_words = 0;
    InitHashTable();
    for (i = 0; i < vocab_size; i++) {
        if (vocab[i].cnt < min_count) {
            vocab_size = i;
            break;
        }
        AddToHashTable(vocab[i].word, i);
        total_words += vocab[i].cnt;
    }
    return total_words;
}

/* Open interfaces */

/*
 * GetWordNum - return words number i.e. vocabulary size
 */
inline int GetWordNum() {
    return vocab_size;
}

/*
 * GetWordIdx - return the result of SearchHashTable(word)
 */
inline int GetWordIdx(char* word) {
    return SearchHashTable(word);
}

/*
 * GetWord - return a word string
 */
inline char* GetWord(int word_idx) {
    return vocab[word_idx].word;
}

/*
 * GetWordCnt - if word is in vocabulary, return its count
 *                else return -1
 */
inline long long GetWordCnt(int word_idx) {
    if (word_idx == NOT_IN_VOCAB)
        return -1;
    else return vocab[word_idx].cnt;
}

/*
 * LearnVocab - learn vocabulary from corpus
 */
long long LearnVocab(char* corpus_path, char* vocab_path, int min_count) {
    FILE* corpus = fopen(corpus_path, "r");
    if (corpus == NULL) 
        ReportError("Can't find vocabulary corpus file");

    InitVocab();
    char word[MAX_WORD_SIZE];
    int word_idx;
    long long train_words = 0;
    // read all words in corpus
    while (1) {
        if (ReadWord(word, corpus) == END_OF_SENTENCE)
            continue;
        if (feof(corpus)) break;
        train_words++;
        if (train_words % 100000 == 0) {
            printf("%lldK%c", train_words / 1000, 13);
            fflush(stdout);
        }
        word_idx = SearchHashTable(word);
        if (word_idx == NOT_IN_HASH_TABLE) {
            word_idx = AddToVocab(word);
            AddToHashTable(word, word_idx);
        }
        vocab[word_idx].cnt++;
    }
    long long total_words = SortVocab(min_count);
    SaveVocab(vocab_path);
    fclose(corpus);
    return total_words;
}

/*
 * LoadVocab - load existed vocabulry
 */
long long LoadVocab(char* vocab_path, int min_count) {
    FILE* fin = fopen(vocab_path, "r");
    if (fin == NULL) 
        ReportError("Vocabulary is not existed!");

    int i, word_num, cnt, word_idx;
    char word[MAX_WORD_SIZE];
    InitVocab();
    fscanf(fin, "%d", &word_num);
    for (i = 0; i < word_num; i++) {
        fscanf(fin, "%s%d", word, &cnt);
        word_idx = AddToVocab(word);
        vocab[word_idx].cnt = cnt;
        AddToHashTable(word, word_idx);
    }
    long long total_words = SortVocab(min_count);
    fclose(fin);
    return total_words;
}

/* 
 * SaveVocab - save vocabulary
 */
void SaveVocab(const char* vocab_path) {
    int i;
    FILE* fout = fopen(vocab_path, "w");
    fprintf(fout, "%d\n", vocab_size);
    for (i = 0; i < vocab_size; i++)
        fprintf(fout, "%s %lld\n", vocab[i].word, vocab[i].cnt);
    fclose(fout);
}

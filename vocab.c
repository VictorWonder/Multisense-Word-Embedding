/********************************************************************************
 *  File Name       : vocab.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-02 16:54
 *  Description     : Interfaces of vocabulary operations
********************************************************************************/

#include "cmg.h"

static vocab_word* vocab;
static int vocab_size = 0;

static void InitVocab();
static int AddToVocab(char*);
static int cmp(const void*, const void*);
static void SortVocab();
static void SaveVocab(char*);

static int cmp(const void* a, const void* b) {
    return ((vocab_word*)a)->cnt < ((vocab_word*)b)->cnt;
}

static void InitVocab() {
    InitHashTable();
    vocab = (vocab_word*)malloc(MAX_VOCAB_SIZE * sizeof(vocab_word));
}

static int AddToVocab(char* word) {
    // copy word
    int len = strlen(word) + 1;
    vocab[vocab_size].word = (char*)malloc(len);
    strcpy(vocab[vocab_size].word, word);
    vocab[vocab_size].word[len] = '\0';
    
    vocab[vocab_size].cnt = 0;
    vocab_size++;
    return vocab_size - 1;
}

static void SortVocab() {
    // sort vocab
    qsort(vocab, vocab_size, sizeof(vocab_word), cmp);
    // rebuild hash table
    int i;
    InitHashTable();
    for (i = 0; i < vocab_size; i++)
        AddToHashTable(vocab[i].word, i);
}

static void SaveVocab(char* vocab_path) {
    /* default path */
    if (vocab_path[0] == '\0')
        vocab_path = "./vocab.txt";

    int i;
    FILE* fout = fopen(vocab_path, "w");
    fprintf(fout, "%d\n", vocab_size);
    for (i = 0; i < vocab_size; i++) 
        fprintf(fout, "%s %d\n", vocab[i].word, vocab[i].cnt);
    fclose(fout);
}

inline int GetWordNum() {
    return vocab_size;
}

inline char* GetWord(register int word_idx) {
    return vocab[word_idx].word;
}

void TrainVocab(char* corpus_path, char* vocab_path) {
    if (corpus_path[0] == '\0')
        corpus_path = "./corpus.txt";
    FILE* corpus = fopen(corpus_path, "r");

    InitVocab();
    char* word = (char*)malloc(MAX_WORD_SIZE);
    int word_idx;
    // read all words in corpus
    while (fscanf(corpus, "%s", word) != EOF) {
        word_idx = SearchHashTable(word);
        if (word_idx == -1) {
            word_idx = AddToVocab(word);
            AddToHashTable(word, word_idx);
        }
        vocab[word_idx].cnt++;
    }
    SortVocab();
    SaveVocab(vocab_path);
    free(word);
    fclose(corpus);
}

void LoadVocab(char* vocab_path) {
    /* default path */
    if (vocab_path[0] == '\0')
        vocab_path = "./vocab.txt";
    FILE* fin = fopen(vocab_path, "r");
    if (fin == NULL) 
        exit(-1);

    int i, word_num, cnt, word_idx;
    InitVocab();
    char* word = (char*)malloc(MAX_WORD_SIZE);
    fscanf(fin, "%d", &word_num);
    for (i = 0; i < word_num; i++) {
        fscanf(fin, "%s%d", word, &cnt);
        word_idx = AddToVocab(word);
        vocab[word_idx].cnt = cnt;
    }
    SortVocab();
    free(word);
    fclose(fin);
}

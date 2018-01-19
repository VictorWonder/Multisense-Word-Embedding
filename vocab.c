/********************************************************************************
 *  File Name       : vocab.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-02 16:54
 *  Description     : Functions for hash table, vocabulary and read
********************************************************************************/

#include "cmg.h"

#define MAX_VOCAB_SIZE 5000000
#define MAX_HASH_TABLE_SIZE 30000000

#define NOT_IN_HASH_TABLE NOT_IN_VOCAB


/* local hash table variables */
const int hash_table_size = MAX_HASH_TABLE_SIZE;
static int* hash_table = NULL;

/* local vocabulary variables */
static vocab_word* vocab = NULL;
static int vocab_size = 0;

/* local hash table functions */
static void InitHashTable();
static unsigned int GetHashKey(char*);
static int SearchHashTable(char*);
static void AddToHashTable(char*, int);

/* local vocabulary functions */
static int WordCntCmp(const void*, const void*);
static void InitVocab();
static int AddToVocab(char*);
static long long SortVocab(int);
static void SaveVocab(char*, int);

/* Local functions */

/* 
 * InitHashTable - if hash table is not initialized, alloc storage for it
 *                 then fill the hash table with -1
 */
static void InitHashTable() {
    if (hash_table == NULL)
        hash_table = (int*)malloc(hash_table_size * sizeof(int));
    memset(hash_table, 0xff, hash_table_size * sizeof(int));
}

/*
 * GetHashKey - get hash key of a word
 */
static unsigned int GetHashKey(char* word) {
    unsigned long long key = 0;
    int len = strlen(word), i;
    for (i = 0; i < len; i++)
        key = key * 257 + word[i];
    return (unsigned int)(key % hash_table_size);
}

/*
 * SearchHashTable - if a word is in hash table, return its index in vocabulary
 *                   else return -1
 */
static int SearchHashTable(char* word) {
    unsigned int key = GetHashKey(word);
    while (1) {
        if (hash_table[key] == -1) 
            return NOT_IN_HASH_TABLE;
        if (!strcmp(word, vocab[hash_table[key]].word))
            return hash_table[key];
        key = (key + 1) % hash_table_size;
    }
    return NOT_IN_HASH_TABLE;
}

/*
 * AddToHashTable - add a word into hash table
 */
static void AddToHashTable(char* word, int word_idx) {
    unsigned int key = GetHashKey(word);
    while (hash_table[key] != -1) 
        key = (key + 1) % hash_table_size;
    hash_table[key] = word_idx;
}

/* 
 * WordCntCmp - compare funtion for word count
 */
static int WordCntCmp(const void* a, const void* b) {
    return ((vocab_word*)a)->cnt < ((vocab_word*)b)->cnt;
}

/* 
 * InitVocab - initialize hash table and then alloc storage for vocabulary
 */
static void InitVocab() {
    InitHashTable();
    vocab = (vocab_word*)malloc(MAX_VOCAB_SIZE * sizeof(vocab_word));
}

/*
 * AddToVocab - add a word into vocabulary
 */
static int AddToVocab(char* word) {
    /* copy the word to vocabulary */
    int len = strlen(word) + 1;
    vocab[vocab_size].word = (char*)calloc(len, sizeof(char));
    strcpy(vocab[vocab_size].word, word);
    
    /* initialize information of this word */
    vocab[vocab_size].cnt = 0;
    vocab[vocab_size].sense_num = 0;

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
    qsort(vocab, vocab_size, sizeof(vocab_word), WordCntCmp);

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

/* 
 * SaveVocab - save vocabulary
 */
static void SaveVocab(char* vocab_path, int min_count) {
    int i;
    FILE* fout = fopen(vocab_path, "w");
    fprintf(fout, "%d\n", vocab_size);
    for (i = 0; i < vocab_size; i++)
        fprintf(fout, "%s %d\n", vocab[i].word, vocab[i].cnt);
    fclose(fout);
}

/* Open interfaces */

/*
 * GetWordNum - return words number i.e. vocabulary size
 */
inline int GetWordNum() {
    return vocab_size;
}

/*
 * GetWord - return the result of SearchHashTable(word)
 */
inline int GetWord(char* word) {
    return SearchHashTable(word);
}

/*
 * GetWordByIdx - return a word string
 */
inline char* GetWordByIdx(int word_idx) {
    return vocab[word_idx].word;
}

/*
 * GetWordCount - if word is in vocabulary, return its count
 *                else return -1
 */
inline int GetWordCount(char* word) {
    int word_idx = SearchHashTable(word);
    if (word_idx == NOT_IN_VOCAB) 
        return -1;
    else return vocab[word_idx].cnt;
}

/*
 * GetWordCountByIdx - if word is in vocabulary, return its count
 *                     else return -1
 */
inline int GetWordCountByIdx(int word_idx) {
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
        ReportError("Corpus file not found");

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
    SaveVocab(vocab_path, min_count);
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
        AddToHashTable(word, word_idx);
        vocab[word_idx].cnt = cnt;
    }
    long long total_words = SortVocab(min_count);
    fclose(fin);
    return total_words;
}
/*
 * ReadWord - read a word from input file
 */
int ReadWord(char* word, FILE* fin) {
    int len = 0;
    char c;
    while (!feof(fin)) {
        c = fgetc(fin);
        if ((c == ' ') || (c == '\t') || (c == '\n')) {
            if (len > 0) {
                if (c == '\n') 
                    ungetc(c, fin);
                break;
            }
            if (c == '\n') 
                return END_OF_SENTENCE;
            else continue;
        }
        word[len] = c;
        len++;
        if (len >= MAX_WORD_SIZE - 1)
            ReportError("Long word!");
    }
    word[len] = 0;
    return 0;
}

int ReadWordIndex(FILE* fin) {
    char word[MAX_WORD_SIZE];
    if (ReadWord(word, fin) == END_OF_SENTENCE)
        return END_OF_SENTENCE;
    else return SearchHashTable(word);
}

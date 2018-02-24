/********************************************************************************
 *  File Name       : hash.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-20 22:33
 *  Description     : Hash table
********************************************************************************/

#include "common.h"
#include "vocab.h"

static int* hash_table = NULL;

/* local hash table functions */
static unsigned int GetHashKey(char*);

/*
 * GetHashKey - get hash key of a word
 */
static unsigned int GetHashKey(char* word) {
    unsigned long long key = 0;
    int len = strlen(word), i;
    for (i = 0; i < len; i++)
        key = key * 257 + word[i];
    return (unsigned int)(key % MAX_HASH_TABLE_SIZE);
}

/* 
 * InitHashTable - if hash table is not initialized, alloc storage for it
 *                 then fill the hash table with -1
 */
void InitHashTable() {
    if (hash_table == NULL)
        hash_table = (int*)malloc(MAX_HASH_TABLE_SIZE * sizeof(int));
    int i;
    for (i = 0; i < MAX_HASH_TABLE_SIZE; i++) hash_table[i] = -1;
}

/*
 * SearchHashTable - if a word is in hash table, return its index in vocabulary
 *                   else return -1
 */
int SearchHashTable(char* word) {
    unsigned int key = GetHashKey(word);
    while (1) {
        if (hash_table[key] == -1) 
            return NOT_IN_HASH_TABLE;
        if (!strcmp(word, GetWord(hash_table[key])))
            return hash_table[key];
        key = (key + 1) % MAX_HASH_TABLE_SIZE;
    }
    return NOT_IN_HASH_TABLE;
}

/*
 * AddToHashTable - add a word into hash table
 */
void AddToHashTable(char* word, int word_idx) {
    unsigned int key = GetHashKey(word);
    while (hash_table[key] != -1) 
        key = (key + 1) % MAX_HASH_TABLE_SIZE;
    hash_table[key] = word_idx;
}

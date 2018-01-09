/********************************************************************************
 *  File Name       : hash.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-09 18:04
 *  Description     : Interfaces of hash table operations
********************************************************************************/

#include "cmg.h"

const int hash_table_size = MAX_VOCAB_SIZE;

static int* hash_table = NULL;

static int GetHashKey(char* word);

static int GetHashKey(char* word) {
    unsigned long long hash = 0;
    int len = strlen(word), i;
    for (i = 0; i < len; i++)
        hash = hash * 257 + word[i];
    hash = hash % hash_table_size;
    return hash;
}

int SearchHashTable(char* word) {
    unsigned int key = GetHashKey(word);
    while (1) {
        if (hash_table[key] == -1) return -1;
        if (!strcmp(word, GetWord(hash_table[key])))
            return hash_table[key];
        key = (key + 1) % hash_table_size;
    }
    return -1;
}

void AddToHashTable(char* word, int word_idx) {
    unsigned int key = GetHashKey(word);
    while (hash_table[key] != -1)
        key = (key + 1) % hash_table_size;
    hash_table[key] = word_idx;
}

void InitHashTable() {
    if (hash_table != NULL) 
        free(hash_table);
    hash_table = (int*)malloc(hash_table_size * sizeof(int));
    memset(hash_table, 0xff, hash_table_size * sizeof(int));
}

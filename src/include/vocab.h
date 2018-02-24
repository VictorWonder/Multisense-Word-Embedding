/********************************************************************************
 *  File Name       : vocab.h
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-20 22:32
 *  Description     : Header for vocabulary and hash table
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_VOCAB_SIZE 30000000
#define MAX_HASH_TABLE_SIZE 30000000

#define MAX_STRING 100
#define MAX_WORD_SIZE 100
#define MAX_SENTENCE_LENGTH 50000

#define NOT_IN_VOCAB -1
#define END_OF_SENTENCE -2
#define NOT_IN_HASH_TABLE NOT_IN_VOCAB

typedef struct {
    char* word;
    long long cnt;
} vocab_word_t;

/* hash.c */
extern void InitHashTable();
extern int SearchHashTable(char*);
extern void AddToHashTable(char*, int);

/* vocab.c */
extern int GetWordNum();
extern int GetWordIdx(char*);
extern char* GetWord(int);
extern long long GetWordCnt(int);
extern long long LearnVocab(char*, char*, int);
extern long long LoadVocab(char*, int);
extern void SaveVocab(const char*);

/* reader.c */
extern long long GetFileSize(char*);
extern int ReadWord(char*, FILE*);
extern int ReadWordIdx(FILE*);
extern int ReadSentence(FILE*, int*, int, double, unsigned long long*);

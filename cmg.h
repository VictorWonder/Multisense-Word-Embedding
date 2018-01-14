/********************************************************************************
 *  File Name       : cmg.h
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-02 15:15
 *  Description     : Header file of the Controled Multisense Generator Toolkit
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <pthread.h>

#define MAX_STRING 100
#define MAX_WORD_SIZE 100
#define MAX_VOCAB_SIZE 3000000

typedef struct {
    char* word;
    int cnt;
} vocab_word;

typedef struct {
    char* word;
    int* vec;
} word_vec;

// hash.c
extern int SearchHashTable(char*);
extern void AddToHashTable(char*, int);
extern void InitHashTable();

// vocab.c
extern int GetWordNum();
extern char* GetWord(int);
extern void TrainVocab(char*, char*);
extern void LoadVocab(char*);

// negative.c
extern void InitNegative();

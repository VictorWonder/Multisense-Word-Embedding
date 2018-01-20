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
//#include <Python.h>

#define MAX_STRING 100
#define MAX_WORD_SIZE 100
#define MAX_SENTENCE_LENGTH 10000

#define NOT_IN_VOCAB -1
#define END_OF_SENTENCE -2

/* word embedding model */
#define SINGLE_SENSE 1
#define MULTI_SENSE 2
#define NP_MULTI_SENSE 3

typedef struct {
    char* word;
    int cnt;
} vocab_word;

typedef struct vlk {
    double* context_vec;
    double* sense_vec;
    struct vlk* next;
    int cnt;
} vec_link;

typedef struct {
    double* global_vec;
    vec_link* sense;
    int sense_num;
} word_vec_t;


/* Global variables */
extern unsigned long long* random_num;

// vocab.c
extern int ReadWord(char*, FILE*);
extern int ReadWordIndex(FILE*);
extern int GetWordNum();
extern int GetWord(char*);
extern char* GetWordByIdx(int);
extern int GetWordCount(char*);
extern int GetWordCountByIdx(int);
extern long long LearnVocab(char*, char*, int);
extern long long LoadVocab(char*, int);

// wordvec.c
extern void InitWordVec(int);
extern void InitNegative(int);
extern void SingleSense(int, int*, int);
extern void MultiSense(int, int*, int);
extern void NonParametricMultiSense(int, int*, int);
extern void SaveWordVec(char*);

// helper.c
extern unsigned long long NextRandomNum(unsigned long long);
extern double RandomRealNum(unsigned long long);
extern void ReportError(char*);
extern void PrintUsage();

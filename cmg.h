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

#define MAX_STRING_LENGTH 100
#define MAX_WORD_SIZE 100
#define MAX_VOCAB_SIZE 3000000

/* structure of vocabulary word */
/* $begin vocab_word */
typedef struct {
    char* word;
    int cnt;
} vocab_word;
/* $end vocab_word */

/* structure of word vector */
/* $begin word_vec */
typedef struct {
    char* word;
    int* vec;
    int dim;
} word_vec;
/* $end word_vec */

extern int SearchHashTable(char* word);
extern void AddToHashTable(char* word, int word_idx);
extern void InitHashTable();

extern char* GetWord(int word_idx);
extern void BuildVocab(FILE* corpus, char* vocab_path);
extern void LoadVocab(char* vocab_path);

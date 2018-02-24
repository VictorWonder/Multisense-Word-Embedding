/********************************************************************************
 *  File Name       : embed.h
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-23 08:43
 *  Description     : 
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/* word embedding sense model */
#define SINGLE_SENSE 1
#define MULTI_SENSE 2
#define NP_MULTI_SENSE 3

typedef struct vl {
    double* context_vec;
    double* sense_vec;
    struct vl* next;
    long long cnt;
    int word_idx;
    int sense_idx;
} vec_list_t;

typedef struct {
    double* global_vec;
    vec_list_t* sense;
    int sense_num;
} word_vec_t;

/* vector parameters */
extern int dim;                         /* vector space dimension */
extern int sense_num;                   /* sense number in multisense embedding and 
                                        maxinum sense number in non-parametric multisense embedding */
/* embedding parameters */
extern word_vec_t* word_vec;            /* word vectors */
extern int word_num;                    /* word number */
extern int context_size;                /* half size of context window */
extern double base_embed_alpha;         /* base learning rate */
extern double embed_alpha;
extern int negative;                    /* number of negative samples */
extern double subsample;                /* subsampling rate */
extern int sense_model;                 /* sense model - SINGLE_SENSE, MULTI_SENSE, NP_MULTI_SENSE */
extern double new_sense_threshold;

/* helper parameters */
extern unsigned long long* random_num;  /* pseudo random number for each thread */

/* wordvec.c */
extern void InitWordVec();
extern void SaveWordVec(char*);
extern void LoadWordVec(char*);
extern void FinishEmbed();

/* optimizer.c */
extern void InitHierarchical();         /* build huffman tree */
extern void HierarchicalSoftmax();
extern void InitNegative();
extern void NegativeSampling(int, double*, int, int);

/* sense.c */
extern vec_list_t* ClosestSense(double*, vec_list_t*);
extern void SingleSense(int, int*, int);
extern void MultiSense(int, int*, int);
extern void NonParametricMultiSense(int, int*, int);

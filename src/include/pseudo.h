/********************************************************************************
 *  File Name       : pseudo.h
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-26 09:43
 *  Description     : Header file for pseudo multisense detection and elimination
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
//#include "wn.h"

#define LIST_BASE_SIZE 100000

#define SUPERVISED 0
#define UNSUPERVISED 1

#define MAX_PAIRS_NUM 500000

extern int total_senses;
extern int pseudo_pairs_num;
extern double** pseudo_pairs[2];
extern int evaluation;

extern double** all_pairs[2];
extern int pairs_num;

/* detect.c */
extern void DetectPseudoSenses(int);
extern void PseudoTransform(double*);

extern void PseudoEliminationEuclid(double*, double*, double*, double);
extern double* TrainMatrix(double**, double**, int, 
        int, double, void (*)(double*, double*, double*, double));
extern void Shrink(double*, double*, double*, double);
extern void Train();

/********************************************************************************
 *  File Name       : score.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-02-06 16:57
 *  Description     : 
********************************************************************************/

#include "common.h"
#include "test.h"

static int* idx;
static double* sorted_data;

static int SpearmanCmp(const void*, const void*);
static void CalcSpearmanRank(double*, int);

static int SpearmanCmp(const void* a, const void* b) {
    return sorted_data[*(int*)a] > sorted_data[*(int*)b];
}

static void CalcSpearmanRank(double* data, int data_size) {
    /* initialize and sort */
    int i, j, k;
    idx = (int*)malloc(data_size * sizeof(int));
    sorted_data = (double*)malloc(data_size * sizeof(double));
    for (i = 0; i < data_size; i++) {
        idx[i] = i;
        sorted_data[i] = data[i];
    }
    qsort(idx, data_size, sizeof(int), SpearmanCmp);
    
    /* calculate rank */
    i = 0;
    while (i < data_size) {
        for (j = i; j < data_size && sorted_data[idx[j]] == sorted_data[idx[i]]; j++);
        for (k = i; k < j; k++) data[idx[k]] = (i + j - 1) / 2.0;
        i = j;
    }
    free(idx);
    free(sorted_data);
}

double Spearman(double* self, double* standard, int data_size) {
    /* rebuild data */
    int i, j;
    for (i = j = 0; i < data_size; i++) {
        if (self[i] == -1 || standard[i] == -1) continue;
        self[j] = self[i];
        standard[j] = standard[i];
        j++;
    }
    data_size = j;
    
    /* calculate score */
    double score = 0.0;
    CalcSpearmanRank(self, data_size);
    CalcSpearmanRank(standard, data_size);
    for (i = 0; i < data_size; i++)
        score += (self[i] - standard[i]) * (self[i] - standard[i]);
    score = score * 6 / data_size / (data_size - 1) / (data_size + 1);
    /* n * (n * n - 1) may be larger than max_int */
    return 1 - score;
}

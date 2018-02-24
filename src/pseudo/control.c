/********************************************************************************
 *  File Name       : control.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-02-22 09:48
 *  Description     : 
********************************************************************************/

#include "common.h"
#include "embed.h"
#include "pseudo.h"

int group_size = 200;

double** all_pairs[2];
double* shrink_rate;
int pairs_num = 0;

static int* chosed_pairs;
static int chosed_pairs_num;

static char* chosed;

void GetAllPairs() {
    fprintf(stdout, "Start getting all pairs...\n");
    all_pairs[0] = (double**)malloc(MAX_PAIRS_NUM * sizeof(double*));
    all_pairs[1] = (double**)malloc(MAX_PAIRS_NUM * sizeof(double*));
    int i;
    vec_list_t* sense[2];
    for (i = 0; i < word_num; i++) {
        for (sense[0] = word_vec[i].sense; sense[0] != NULL; sense[0] = sense[0]->next) {
            for (sense[1] = sense[0]->next; sense[1] != NULL; sense[1] = sense[1]->next) {
                all_pairs[0][pairs_num] = sense[0]->sense_vec;
                all_pairs[1][pairs_num] = sense[1]->sense_vec;
                pairs_num++;
            }
        }
    }
    fprintf(stdout, "End getting all pairs.\n");
}

void Shrink(double* matrix, double* vec_x, double* vec_y, double alpha) {
    int i, j;
    double tmp_x, tmp_y;
    double* mat_adj = (double*)calloc(dim * dim, sizeof(double));
    for (i = 0; i < dim; i++) {
        tmp_x = 0;
        tmp_y = 0;
        for (j = 0; j < dim; j++) {
            tmp_x += matrix[i * dim + j] * vec_x[j];
            tmp_y += matrix[i * dim + j] * vec_y[j];
        }
        for (j = 0; j < dim; j++) 
            mat_adj[i * dim + j] += (tmp_x * vec_x[j] + tmp_y * vec_y[j] - (vec_x[j] + vec_y[j]) * vec_y[i]) * 2;
    }
    for (i = 0; i < dim * dim; i++)
        matrix[i] += mat_adj[i];
    free(mat_adj);
}

void Train() {
    GetAllPairs();
    if (pairs_num < group_size) group_size = pairs_num;
    chosed = (char*)malloc(pairs_num * sizeof(char));
    chosed_pairs = (int*)calloc(group_size, sizeof(int));
    shrink_rate = (double*)malloc(pairs_num * sizeof(double));
    while (1) {
        int x = rand() % pairs_num;
        if (!chosed[x]) {
            chosed[x] = 1;
            chosed_pairs[chosed_pairs_num] = x;
            chosed_pairs_num++;
            if (chosed_pairs_num == group_size) break;
        }
    }
    int rnd, i;
    for (rnd = 0; rnd < 10; rnd++) {
        double** pairs_x = (double**)malloc(group_size * sizeof(double*));
        double** pairs_y = (double**)malloc(group_size * sizeof(double*));
        for (i = 0; i < group_size; i++) {
            pairs_x[i] = all_pairs[0][chosed_pairs[i]];
            pairs_y[i] = all_pairs[1][chosed_pairs[i]];
        }
        double* matrix = TrainMatrix(pairs_x, pairs_y, group_size, 100, 0.001, Shrink);
        
    }
    free(chosed);
    free(chosed_pairs);
}

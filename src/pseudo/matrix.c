/********************************************************************************
 *  File Name       : transform.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-26 17:58
 *  Description     : 
********************************************************************************/

#include "common.h"
#include "embed.h"
#include "pseudo.h"

/*
void grad_model_cosin(double* mat, double* vec, double* mean, double* mat_adj, int vec_dim, double alpha) {
    //delta a_{i,j} = vec_j * (mean_i * trans_mold - A_row_i * vec * sum / trans_mold) / trans_mold ^ 2 / mean_mold
    double* trans = (double*)mallo; // A*vec
    double sum = 0.0; // (A*vec)'*mean
    for (int i = 0; i < vec_dim; i++) {
        trans[i] = 0.0;
        for (int j = 0; j < vec_dim; j++)
            trans[i] += mat[mat_pos(i, j, vec_dim)] * vec[j];
        sum += mean[i] * trans[i];
    }
    double trans_mold = vec_mold(trans, vec_dim);
    double mean_mold = vec_mold(mean, vec_dim);
    for (int i = 0; i < vec_dim; i++) { // for each row
        for (int j = 0; j < vec_dim; j++)
            mat_adj[mat_pos(i, j, vec_dim)] = alpha * vec[j] * ((mean[i] * trans_mold - trans[i] * sum/ trans_mold) / (trans_mold * trans_mold)) / mean_mold;
    }
    delete[] trans;
}*/

void PseudoEliminationEuclid(double* mat, double* vec_x, double* vec_y, double alpha) {
    int i, j;
    double dot, mean;
    double* mat_adj = (double*)malloc(dim * dim * sizeof(double));
    for (i = 0; i < dim * dim; i++) mat_adj[i] = 0;
    for (i = 0; i < dim; i++) {
        mean = (vec_x[i] + vec_y[i]) / 2.0;

        dot = 0.0;
        for (j = 0; j < dim; j++) 
            dot += mat[i * dim + j] * vec_x[j];
        dot -= mean;
        for (j = 0; j < dim; j++) 
            mat_adj[i * dim + j] = 2 * vec_x[j] * dot * alpha;

        dot = 0.0;
        for (j = 0; j < dim; j++) 
            dot += mat[i * dim + j] * vec_y[j];
        dot -= mean;
        for (j = 0; j < dim; j++) 
            mat_adj[i * dim + j] = 2 * vec_y[j] * dot * alpha;
    }
    for (i = 0; i < dim * dim; i++) mat[i] -= mat_adj[i];
    free(mat_adj);
}

double* TrainMatrix(double** dataset_x, double** dataset_y, int dataset_size, 
        int max_round, double alpha, void (*loss_func)(double*, double*, double*, double)) {

    /* initialize matrix */
    int i, round;
    double* matrix = RandomNewMat(dim, dim, &next_random);
    
    for (round = 0; round < max_round; round++) {
        if (round % (max_round / 10) == 0) {
            fprintf(stdout, "Training matrix round: %d\n", round);
            if (round != 0) alpha *= 0.8;
        }
        for (i = 0; i < dataset_size; i++)
            loss_func(matrix, dataset_x[i], dataset_y[i], alpha);
    }
    return matrix;
}

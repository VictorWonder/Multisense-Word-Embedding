/********************************************************************************
 *  File Name       : detect.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-26 09:53
 *  Description     : Detect pseudo multisense pairs
********************************************************************************/

#include "common.h"
#include "ds.h"
#include "embed.h"
#include "pseudo.h"

int pseudo_list_size = LIST_BASE_SIZE;
int pseudo_pairs_num = 0;
double** pseudo_pairs[2];
int top_k = 20;
double threshold = 5;
int evaluation = UNSUPERVISED;

static double** knn_vec[2];

static int (*eval_func)(double*, double*);

static int Unsupervised(double*, double*);
static int Supervised(double*, double*);

static int Unsupervised(double* vec_a, double* vec_b) {
    int i, j;
    double sum = 0.0;
    int top_k0 = KNearestNeighbor(knn_vec[0], total_senses, vec_a, top_k, dim);
    int top_k1 = KNearestNeighbor(knn_vec[1], total_senses, vec_b, top_k, dim);
    for (i = 0; i < top_k0; i++)
        for (j = 0; j < top_k1; j++) 
            sum += CosinDist(knn_vec[0][i], knn_vec[1][j], dim);
    return sum > threshold;
}

static int Supervised(double* vec_a, double* vec_b) {
    return 0;
}

void DetectPseudoSenses(int word_num) {
    int i;
    vec_list_t* sense[2];
    knn_vec[0] = (double**)malloc(MAX_NEIGHBOR_NUM * sizeof(double*));
    knn_vec[1] = (double**)malloc(MAX_NEIGHBOR_NUM * sizeof(double*));
    pseudo_pairs[0] = (double**)malloc(pseudo_list_size * sizeof(double*));
    pseudo_pairs[1] = (double**)malloc(pseudo_list_size * sizeof(double*));

    if (evaluation == UNSUPERVISED) eval_func = (int (*)(double*, double*))Unsupervised;
    else if (evaluation == SUPERVISED) eval_func = (int (*)(double*, double*))Supervised;

    for (i = 0; i < word_num; i++) {
        if (i % 1000 == 0) {
            fprintf(stdout, "detection process: %.6f%%%c", (double)i / word_num * 100, 13);
            fflush(stdout);
        }
        for (sense[0] = word_vec[i].sense; sense[0] != NULL; sense[0] = sense[0]->next) {
            for (sense[1] = sense[0]->next; sense[1] != NULL; sense[1] = sense[1]->next) {
                if (eval_func(sense[0]->sense_vec, sense[1]->sense_vec)) {
                    pseudo_pairs[0][pseudo_pairs_num] = sense[0]->sense_vec;
                    pseudo_pairs[1][pseudo_pairs_num] = sense[1]->sense_vec;
                    pseudo_pairs_num++;
                    if (pseudo_pairs_num >= pseudo_list_size) {
                        pseudo_list_size += LIST_BASE_SIZE;
                        pseudo_pairs[0] = (double**)realloc(pseudo_pairs[0], pseudo_list_size * sizeof(double*));
                        pseudo_pairs[1] = (double**)realloc(pseudo_pairs[1], pseudo_list_size * sizeof(double*));
                    }
                }
            }
        }
    }
    fprintf(stdout, "Pseudo sense detection is successful\n");
    fprintf(stdout, "Find %d pairs pseudo senses\n", pseudo_pairs_num);
    free(knn_vec[0]);
    free(knn_vec[1]);
    return;
}

void PseudoTransform(double* matrix) {
    int i;
    vec_list_t* sense;
    for (i = 0; i < word_num; i++) {
        for (sense = word_vec[i].sense; sense != NULL; sense = sense->next)
            MatDotVec(matrix, sense->sense_vec, dim, dim);
    }
}

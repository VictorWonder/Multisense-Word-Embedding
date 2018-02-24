/********************************************************************************
 *  File Name       : optimizer.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-24 10:27
 *  Description     : 
********************************************************************************/

#include "common.h"
#include "vocab.h"
#include "embed.h"
#include "ds.h"

#define MAX_CODE_LENGTH 24    /* max length of huffman code */

/* initialization flag */
static int neg_para_init = 0;
static int hie_para_init = 0;

static double* para_vec;

/* variables for hierarchical softmax huffman tree */
static int* code_length;
static char** huffman_code;
static long long** huffman_point;

/* variables for negative sampling table */
const int table_size = 1e8;
static int* sample_table;

/* Hierarchical Softmax */

/* 
 * InitHierarchical - initialize parameters of hierarchical softmax
 */
void InitHierarchical() {
    if (word_num == 0) ReportError("Vocabulary is not existed!");
    if (hie_para_init) return;

    long long tree_size = (word_num << 1) | 1;
    long long i, j, k, array_len = tree_size * dim;
    para_vec = (double*)calloc(array_len, sizeof(double));

    long long pos1 = word_num - 1, pos2 = word_num, min1i, min2i;
    long long* count = (long long*)calloc(tree_size, sizeof(long long));
    long long* tree = (long long*)calloc(tree_size, sizeof(long long));
    long long* parent_node = (long long*)calloc(tree_size, sizeof(long long));
    char code[MAX_CODE_LENGTH];
    long long point[MAX_CODE_LENGTH];
    code_length = (int*)calloc(word_num, sizeof(int));
    huffman_code = (char**)calloc(word_num, sizeof(char*));
    huffman_point = (long long**)calloc(word_num, sizeof(long long*));
    for (i = 0; i < word_num; i++) count[i] = GetWordCnt(i);
    for (i = word_num; i < tree_size; i++) count[i] = 1e15;

    for (i = 0; i < word_num - 1; i++) {
        if (pos1 >= 0 && count[pos1] < count[pos2]) min1i = pos1--;
        else min1i = pos2++;
        if (pos1 >= 0 && count[pos1] < count[pos2]) min2i = pos1--;
        else min2i = pos2++;
        count[word_num + i] = count[min1i] + count[min2i];
        parent_node[min1i] = parent_node[min2i] = word_num + i;
        tree[min2i] = 1;            /* because of calloc, tree[min1i] has been 0 */
    }
    for (i = 0; i < word_num; i++) {
        for (j = i, k = 0; j != word_num * 2 - 2; k++, j = parent_node[j]) {
            code[k] = tree[j];
            point[k] = j;
        }

        code_length[i] = k;
        huffman_code[i] = (char*)calloc(k, sizeof(char));
        huffman_point[i] = (long long*)calloc(k, sizeof(long long));
        for (j = 0; j < k; j++) {
            huffman_code[i][k - j - 1] = code[j];
            huffman_point[i][k - j] = point[j] - word_num;
        }
    }
    free(count);
    free(tree);
    free(parent_node);
    hie_para_init = 1;
}

void HierarchicalSoftmax(int id, double* goal_vec, int goal_word, int para_word) {
    if (!hie_para_init) ReportError("Initialize hierarchical softmax failed!");

    /* initialize */
    int i, d;
    double rate;
    double* para_vec_base;
    double* update_vec = (double*)calloc(dim, sizeof(double));

    for (i = 0; i < code_length[goal_word]; i++) {
        rate = 0;
        para_vec_base = para_vec + huffman_point[goal_word][i] * dim;
        for (d = 0; d < dim; d++) rate += goal_vec[d] * para_vec_base[d];
        rate = (1 - huffman_code[goal_word][i] - 1.0 / (1.0 + exp(-rate))) * embed_alpha;
        for (d = 0; d < dim; d++) {
            update_vec[d] += rate * para_vec_base[d];
            para_vec_base[d] += rate * goal_vec[d];
        }
    }
    for (d = 0; d < dim; d++) goal_vec[d] += update_vec[d];
    free(update_vec);
}

void InitNegative() {
    if (word_num == 0) ReportError("Vocabulary is not existed!");
    if (neg_para_init) return;

    long long i, array_len = (long long)word_num * dim, j;
    para_vec = (double*)calloc(array_len, sizeof(double));

    /* initialize negative sampling table */
    double total_words_pow = 0;
    double power = 0.75, p;
    sample_table = (int*)malloc(table_size * sizeof(int));
    for (i = 0; i < word_num; i++)
        total_words_pow += pow(GetWordCnt(i), power);
    
    i = 0;
    p = pow(GetWordCnt(i), power) / total_words_pow;
    for (j = 0; j < table_size; j++) {
        sample_table[j] = i;
        if (j / (double)table_size > p) {
            i++;
            p += pow(GetWordCnt(i), power) / total_words_pow;
        }
        if (i >= word_num)
            i = word_num - 1;
    }

    neg_para_init = 1;
}

void NegativeSampling(int id, double* goal_vec, int goal_word, int para_word) {
    if (!neg_para_init) ReportError("Initialize negative sampling failed!");

    /* initialize */
    int i, d, target, label;
    double rate;
    double* para_vec_base;
    double* update_vec = (double*)calloc(dim, sizeof(double));

    /* negative sampling */
    for (i = 0; i < negative + 1; i++) {
        // TODO: para_word and goal_word is different from google trunk
        /* positive example */
        if (i == 0) {
            target = para_word;
            label = 1;
        } 
        /* negative example */
        else {
            /* if random negative example is current word */
            do {
                NextRandomNum(random_num + id);
                target = sample_table[(random_num[id] >> 16) % table_size];
            } while (target == goal_word);
            label = 0;
        }
        para_vec_base = para_vec + target * dim;

        /* calculate learning rate */
        rate = 0.0;
        for (d = 0; d < dim; d++) 
            rate += goal_vec[d] * para_vec_base[d];
        rate = (label - 1.0 / (1.0 + exp(-rate))) * embed_alpha;

        /* update word vector and negative parameters */
        for (d = 0; d < dim; d++) {
            update_vec[d] += rate * para_vec_base[d];
            para_vec_base[d] += rate * goal_vec[d];
        }
    }
    for (d = 0; d < dim; d++) goal_vec[d] += update_vec[d];
    free(update_vec);
}

void FinishEmbed() {
    free(para_vec);
    if (hie_para_init) {
        int i;
        for (i = 0; i < word_num; i++) {
            free(huffman_code[i]);
            free(huffman_point[i]);
        }
        free(huffman_code);
        free(huffman_point);
    }
    if (neg_para_init) free(sample_table);
}

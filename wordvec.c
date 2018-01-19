/********************************************************************************
 *  File Name       : wordvec.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-16 15:37
 *  Description     : 
********************************************************************************/

#include "cmg.h"

/* initialization flags */
static int word_vec_init = 0;
static int neg_para_init = 0;

/* word vectors and negative parameters */
static double* word_vec;
static double* neg_para;

extern int dim;
extern int window;
extern double alpha;
extern int negative;

static double* update;

const int table_size = 1e8;
static int* table;

static double CalcSigma(double);

static inline double CalcSigma(register double x) {
    return 1.0 / (1 + exp(-x));
}

void InitWordVec(int dim) {
    int word_num = GetWordNum();
    if (word_num == 0)
        ReportError("Vocabulary is not existed!");

    if (word_vec_init) 
        return;

    int i, array_len = word_num * dim;
    unsigned long long random_num = 1; // (unsigned long long)time(NULL);
    word_vec = (double*)malloc(array_len * sizeof(double));
    update = (double*)malloc(dim * sizeof(double));
    for (i = 0; i < array_len; i++) {
        random_num = NextRandomNum(random_num);
        word_vec[i] = RandomRealNum(random_num) - 0.5; // / dim
    }
    word_vec_init = 1;
}

void InitNegative(int dim) {
    int word_num = GetWordNum();
    if (word_num == 0) 
        ReportError("Vocabulary is not existed!");

    if (neg_para_init) 
        return;

    int i, array_len = word_num * dim, j;
    unsigned long long random_num = (unsigned long long)time(NULL);
    neg_para = (double*)malloc(array_len * sizeof(double));
    for (i = 0; i < array_len; i++) {
        random_num = NextRandomNum(random_num);
        neg_para[i] = RandomRealNum(random_num) - 0.5;
    }
    neg_para_init = 1;

    /* initialize negative sampling table */
    double total_words_pow = 0;
    double power = 0.75, p;
    table = (int*)malloc(table_size * sizeof(int));
    for (i = 0; i < word_num; i++)
        total_words_pow += pow(GetWordCountByIdx(i), power);
    
    i = 0;
    p = pow(GetWordCountByIdx(i), power) / total_words_pow;
    for (j = 0; j < table_size; j++) {
        table[j] = i;
        if (j / (double)table_size > p) {
            i++;
            p += pow(GetWordCountByIdx(i), power) / total_words_pow;
        }
        if (i >= word_num)
            i = word_num - 1;
    }
}

void SingleSense(int id, int* sen, int sentence_length) {
    /* base address of word vectors and negative parameters */
    long long word_vec_base;
    long long neg_para_base;

    int cur_word, word, i;
    int d; /* vector iterator */
    int window_size; /* real window size which is a random integer between 1 and window */
    int target, label;
    double rate; /* real learning rate */

    /* update word vector of each word in the sentence */
    for (cur_word = 0; cur_word < sentence_length; cur_word++) {
        /* real context window size */
        random_num[id] = NextRandomNum(random_num[id]);
        window_size = random_num[id] % window + 1;

        /* initialize */
        word_vec_base = sen[cur_word] * dim;
        for (d = 0; d < dim; d++)
            update[d] = 0;

        /* for every word in context window */
        for (word = cur_word - window_size; word <= cur_word + window_size; word++) {
            /* if out of range */
            if (word < 0) continue;
            if (word >= sentence_length) break;
            if (word == cur_word) continue;

            /* negative sampling */
            for (i = 0; i < negative + 1; i++) {
                /* positive example */
                if (i == 0) {
                    target = sen[word];
                    label = 1;
                } 
                /* negative example */
                else {
                    /* if random negative example is current word */
                    do {
                        random_num[id] = NextRandomNum(random_num[id]);
                        target = table[(random_num[id] >> 16) % table_size];
                    } while (target == sen[word]);
                    label = 0;
                }
                neg_para_base = target * dim;
                
                /* calculate learning rate */
                rate = 0.0;
                for (d = 0; d < dim; d++) 
                    rate += word_vec[word_vec_base + d] * neg_para[neg_para_base + d];
                rate = (label - CalcSigma(rate)) * alpha;
                
                /* update word vector and negative parameters */
                for (d = 0; d < dim; d++) {
                    update[i] += rate * neg_para[neg_para_base + d];
                    neg_para[neg_para_base + d] += rate * word_vec[word_vec_base + d];
                }
            }
            for (d = 0; d < dim; d++) word_vec[word_vec_base + d] += update[d];
        }
    }
}

void MultiSense(int id, int* sen, int sentence_length) {
}

void NonParametricMultiSense(int id, int* sen, int sentence_length){
}

void SaveWordVec(char* vector_path) {
    FILE* fout = fopen(vector_path, "w");
    if (fout == NULL)
        ReportError("Failed to save word vectors");

    int i, d, word_num = GetWordNum();
    long long word_vec_base = 0;
    fprintf(fout, "%d\n", word_num);
    for (i = 0; i < word_num; i++, word_vec_base += dim) {
        fprintf(fout, "%s ", GetWordByIdx(i));
        for (d = 0; d < dim; d++)
            fprintf(fout, "%.6f ", word_vec[word_vec_base + d]);
        fprintf(fout, "\n");
    }
    fclose(fout);
}

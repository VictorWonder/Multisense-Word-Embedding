/********************************************************************************
 *  File Name       : sense.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-23 15:32
 *  Description     : 
********************************************************************************/

#include "common.h"
#include "vocab.h"
#include "embed.h"

static double* context_vec;

double new_sense_threshold = 0;

vec_list_t* ClosestSense(double* context_vec, vec_list_t* sense) {
    if (sense == NULL)
        return NULL;

    vec_list_t* result = sense;
    double min_dist = CosinDist(context_vec, sense->context_vec, dim);
    for (sense = sense->next; sense != NULL; sense = sense->next) {
        double dist = CosinDist(context_vec, sense->context_vec, dim);
        if (dist < min_dist) {
            min_dist = dist;
            result = sense;
        }
    }
    return result;
}

void SingleSense(int id, int* sen, int sentence_length) {
    int i, j; /*iterator */
    int cur_word, context_word;
    int real_context_size; /* real context size which is a random integer between 1 and context_size */

    /* update word vector of each word in the sentence */
    for (i = 0; i < sentence_length; i++) {
        cur_word = sen[i];

        /* real context size */
        NextRandomNum(random_num + id);
        real_context_size = random_num[id] % context_size;

        /* for every word in context */
        for (j = i - real_context_size; j <= i + real_context_size; j++) {
            /* if out of range */
            if (j < 0) continue;
            if (j >= sentence_length) break;
            if (j == i) continue;
            context_word = sen[j];
            if (context_word < 0) continue;

            NegativeSampling(id, word_vec[context_word].global_vec, context_word, cur_word);
        }
    }
}

void MultiSense(int id, int* sen, int sentence_length) {
    int i, j, d; /*iterator */
    int cur_word, context_word;
    int real_context_size; /* real context size which is a random integer between 1 and context_size */
    int cnt;
    vec_list_t* sense;
    double* context_vec_ptr;
    if (context_vec == NULL)
        context_vec = (double*)malloc(dim * sizeof(double));

    /* update word vector of each word in the sentence */
    for (i = 0; i < sentence_length; i++) {
        cur_word = sen[i];

        /* real context size */
        NextRandomNum(random_num + id);
        real_context_size = random_num[id] % context_size + 1;

        /* get average vector of context */
        cnt = 0;
        for (d = 0; d < dim; d++) context_vec[d] = 0;
        for (j = i - real_context_size; j <= i + real_context_size; j++) {
            /* if out of range */
            if (j < 0) continue;
            if (j >= sentence_length) break;
            if (j == i) continue;
            
            /* sum of context vectors */
            context_word = sen[j];
            for (d = 0; d < dim; d++) 
                context_vec[d] += word_vec[context_word].global_vec[d];
            cnt++;
        }
        for (d = 0; d < dim; d++)
            context_vec[d] /= cnt;

        sense = ClosestSense(context_vec, word_vec[cur_word].sense);
        context_vec_ptr = sense->context_vec;
        for (d = 0; d < dim; d++) 
            context_vec_ptr[d] = (context_vec_ptr[d] * sense->cnt + context_vec[d]) / (sense->cnt + 1);
        sense->cnt += 1;

        /* for every word in context window */
        for (j = i - real_context_size; j <= i + real_context_size; j++) {
            /* if out of range */
            if (j < 0) continue;
            if (j >= sentence_length) break;
            if (j == i) continue;
            context_word = sen[j];

            NegativeSampling(id, word_vec[cur_word].global_vec, cur_word, context_word);
            NegativeSampling(id, sense->sense_vec, cur_word, context_word);
        }
    }
}

void NonParametricMultiSense(int id, int* sen, int sentence_length) {
    int i, j, d; /*iterator */
    int cur_word, context_word;
    int real_context_size; /* real context size which is a random integer between 1 and context_size */
    int cnt;
    vec_list_t* sense;
    double* context_vec_ptr;
    if (context_vec == NULL)
        context_vec = (double*)malloc(dim * sizeof(double));

    /* update word vector of each word in the sentence */
    for (i = 0; i < sentence_length; i++) {
        cur_word = sen[i];

        /* real context size */
        NextRandomNum(random_num + id);
        real_context_size = random_num[id] % context_size + 1;

        /* get average vector of context */
        cnt = 0;
        for (d = 0; d < dim; d++) context_vec[d] = 0;
        for (j = i - real_context_size; j <= i + real_context_size; j++) {
            /* if out of range */
            if (j < 0) continue;
            if (j >= sentence_length) break;
            if (j == i) continue;
            
            /* sum of context vectors */
            context_word = sen[j];
            for (d = 0; d < dim; d++) 
                context_vec[d] += word_vec[context_word].global_vec[d];
            cnt++;
        }
        for (d = 0; d < dim; d++)
            context_vec[d] /= cnt;

        sense = ClosestSense(context_vec, word_vec[cur_word].sense);
        if ((word_vec[cur_word].sense_num < sense_num || word_vec[cur_word].sense_num < (int)log(GetWordCnt(cur_word)))
            && (sense == NULL || CosinDist(context_vec, sense->context_vec, dim) < new_sense_threshold)) {
            vec_list_t* new_sense = (vec_list_t*)malloc(sizeof(vec_list_t));
            new_sense->context_vec = (double*)calloc(dim, sizeof(double));
            new_sense->sense_vec = RandomNewVec(dim, random_num + id);
            new_sense->cnt = 0;
            new_sense->word_idx = cur_word;
            new_sense->sense_idx = word_vec[cur_word].sense_num;
            new_sense->next = word_vec[cur_word].sense;
            word_vec[cur_word].sense = new_sense;
            word_vec[cur_word].sense_num++;
            sense = new_sense;
        }
        context_vec_ptr = sense->context_vec;
        for (d = 0; d < dim; d++) 
            context_vec_ptr[d] = (context_vec_ptr[d] * sense->cnt + context_vec[d]) / (sense->cnt + 1);
        sense->cnt += 1;

        /* for every word in context window */
        for (j = i - real_context_size; j <= i + real_context_size; j++) {
            /* if out of range */
            if (j < 0) continue;
            if (j >= sentence_length) break;
            if (j == i) continue;
            context_word = sen[j];

            NegativeSampling(id, word_vec[cur_word].global_vec, cur_word, context_word);
            NegativeSampling(id, sense->sense_vec, cur_word, context_word);
        }
    }
}


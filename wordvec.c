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
static word_vec_t* word_vec;
static double* neg_para;

extern int dim;
extern int window;
extern double alpha;
extern int negative;
extern int sense_num;
extern int model;

static double* update_vec;
static double* context_vec;

const int table_size = 1e8;
static int* table;

static double CalcSigma(double);
static double VectorMold(double*);
static double EuclidDist(double*, double*);
static double CosinDist(double*, double*);
static void NegativeSampling(int, double*, int, int);
static vec_link* ClosestSense(double*, vec_link*);

static inline double CalcSigma(register double x) {
    return 1.0 / (1 + exp(-x));
}

static inline double VectorMold(double* vec) {
    int d;
    double sum = 0;
    for (d = 0; d < dim; d++)
        sum += vec[d] * vec[d];
    return sqrt(sum);
}

static inline double EuclidDist(double* vec_a, double* vec_b) {
    int d;
    double sum = 0;
    for (d = 0; d < dim; d++) 
        sum += (vec_a[d] - vec_b[d]) * (vec_a[d] - vec_b[d]);
    return sqrt(sum);
}

static inline double CosinDist(double* vec_a, double* vec_b) {
    int d;
    double sum = 0;
    for (d = 0; d < dim; d++) 
        sum += vec_a[d] * vec_b[d];
    return sum / (VectorMold(vec_a) * VectorMold(vec_b));
}

static inline void NegativeSampling(int id, double* goal_vec, int goal_word, int context_word) {
    /* initialize */
    int i, d, target, label;
    double* neg_para_base;
    double rate;
    for (d = 0; d < dim; d++)
        update_vec[d] = 0;

    /* negative sampling */
    for (i = 0; i < negative + 1; i++) {
        // TODO: context_word and goal_word is different from google trunk
        /* positive example */
        if (i == 0) {
            target = context_word;
            label = 1;
        } 
        /* negative example */
        else {
            /* if random negative example is current word */
            do {
                random_num[id] = NextRandomNum(random_num[id]);
                target = table[(random_num[id] >> 16) % table_size];
            } while (target == goal_word);
            label = 0;
        }
        neg_para_base = neg_para + target * dim;

        /* calculate learning rate */
        rate = 0.0;
        for (d = 0; d < dim; d++) 
            rate += goal_vec[d] * neg_para_base[d];
        rate = (label - CalcSigma(rate)) * alpha;

        /* update word vector and negative parameters */
        for (d = 0; d < dim; d++) {
            update_vec[d] += rate * neg_para_base[d];
            neg_para_base[d] += rate * goal_vec[d];
        }
    }
    for (d = 0; d < dim; d++) goal_vec[d] += update_vec[d];
}

static vec_link* ClosestSense(double* context_vec, vec_link* sense) {
    vec_link* result = sense;
    double min_dist = CosinDist(context_vec, sense->context_vec);
    for (sense = sense->next; sense != NULL; sense = sense->next) {
        double dist = CosinDist(context_vec, sense->context_vec);
        if (dist < min_dist) {
            min_dist = dist;
            result = sense;
        }
    }
    return result;
}

void InitWordVec(int dim) {
    int word_num = GetWordNum();
    if (word_num == 0)
        ReportError("Vocabulary is not existed!");

    if (word_vec_init) 
        return;

    int i, k, d;
    unsigned long long random_num = 1; // (unsigned long long)time(NULL);
    vec_link* new_sense = NULL;
    word_vec = (word_vec_t*)malloc(word_num * sizeof(word_vec_t));
    update_vec = (double*)malloc(dim * sizeof(double));
    context_vec = (double*)malloc(dim * sizeof(double));
    for (i = 0; i < word_num; i++) {
        word_vec[i].global_vec = (double*)malloc(dim * sizeof(double));
        if (model == MULTI_SENSE) {
            word_vec[i].sense_num = sense_num;
            word_vec[i].sense = NULL;
            for (k = 0; k < sense_num; k++) {
                new_sense = (vec_link*)malloc(sizeof(vec_link));
                new_sense->context_vec = (double*)calloc(dim, sizeof(double));
                new_sense->sense_vec = (double*)malloc(dim * sizeof(double));
                for (d = 0; d < dim; d++) {
                    random_num = NextRandomNum(random_num);
                    new_sense->sense_vec[d] = (RandomRealNum(random_num) - 0.5) / dim;
                }
                new_sense->cnt = 0;
                new_sense->next = word_vec[i].sense;
                word_vec[i].sense = new_sense;
            }
        } else {
            word_vec[i].sense = NULL;
            word_vec[i].sense_num = 0;
        }
        for (d = 0; d < dim; d++) {
            random_num = NextRandomNum(random_num);
            word_vec[i].global_vec[d] = (RandomRealNum(random_num) - 0.5) / dim;
        }
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
        neg_para[i] = (RandomRealNum(random_num) - 0.5) / dim;
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
    int i, j; /*iterator */
    int cur_word, context_word;
    int window_size; /* real window size which is a random integer between 1 and window */

    /* update word vector of each word in the sentence */
    for (i = 0; i < sentence_length; i++) {
        cur_word = sen[i];

        /* real context window size */
        random_num[id] = NextRandomNum(random_num[id]);
        window_size = random_num[id] % window + 1;

        /* for every word in context window */
        for (j = i - window_size; j <= i + window_size; j++) {
            /* if out of range */
            if (j < 0) continue;
            if (j >= sentence_length) break;
            if (j == i) continue;
            context_word = sen[j];

            NegativeSampling(id, word_vec[cur_word].global_vec, cur_word, context_word);
        }
    }
}

void MultiSense(int id, int* sen, int sentence_length) {
    int i, j, d; /*iterator */
    int cur_word, context_word;
    int window_size; /* real window size which is a random integer between 1 and window */
    int cnt;
    vec_link* sense;
    double* context_vec_ptr;

    /* update word vector of each word in the sentence */
    for (i = 0; i < sentence_length; i++) {
        cur_word = sen[i];

        /* real context window size */
        random_num[id] = NextRandomNum(random_num[id]);
        window_size = random_num[id] % window + 1;

        /* get average vector of context */
        cnt = 0;
        for (d = 0; d < dim; d++) context_vec[d] = 0;
        for (j = i - window_size; j <= i + window_size; j++) {
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

        /* for every word in context window */
        for (j = i - window_size; j <= i + window_size; j++) {
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
    int window_size; /* real window size which is a random integer between 1 and window */
    int cnt;
    vec_link* sense;
    double* context_vec_ptr;

    /* update word vector of each word in the sentence */
    for (i = 0; i < sentence_length; i++) {
        cur_word = sen[i];

        /* real context window size */
        random_num[id] = NextRandomNum(random_num[id]);
        window_size = random_num[id] % window + 1;

        /* get average vector of context */
        cnt = 0;
        for (d = 0; d < dim; d++) context_vec[d] = 0;
        for (j = i - window_size; j <= i + window_size; j++) {
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

        /* for every word in context window */
        for (j = i - window_size; j <= i + window_size; j++) {
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

void SaveWordVec(char* vector_path) {
    FILE* fout = fopen(vector_path, "w");
    if (fout == NULL)
        ReportError("Failed to save word vectors");

    int i, d, word_num = GetWordNum();
    double* word_vec_base;
    fprintf(fout, "%d\n", word_num);
    for (i = 0; i < word_num; i++) {
        fprintf(fout, "%s ", GetWordByIdx(i));
        word_vec_base = word_vec[i].global_vec;
        for (d = 0; d < dim; d++) 
            fprintf(fout, "%.6f ", word_vec_base[d]);
        fprintf(fout, "\n");
    }
    fclose(fout);
}

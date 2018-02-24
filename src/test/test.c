/********************************************************************************
 *  File Name       : test.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-02-09 11:52
 *  Description     : 
********************************************************************************/

#include "common.h"
#include "vocab.h"
#include "embed.h"
#include "ds.h"
#include "test.h"

#define SCWS_FILE_PATH "../data/test/ratings.txt"
#define WORDSIM353_FILE_PATH "../data/test/wordsim353.txt"
#define ANALOGY_SEMANTIC_FILE_PATH "../data/test/semantic.txt"
#define ANALOGY_SYNTACTIC_FILE_PATH "../data/test/syntactic.txt"

static int GetContext(FILE*, double*);
static int TestAnalogyQues(double*, int*, int);
static void TestAnalogySub(char*);

static int GetContext(FILE* test_file, double* context) {
    int i, d, word_idx;
    int context_size;
    int context_cnt = 0;
    char word[MAX_WORD_SIZE];
    double* vec;
    for (d = 0; d < dim; d++) context[d] = 0;
    fscanf(test_file, "%d", &context_size);
    for (i = 0; i < context_size; i++) {
        fscanf(test_file, "%s", word);
        word_idx = GetWordIdx(word);
        if (word_idx == NOT_IN_VOCAB) continue;
        context_cnt++;
        vec = word_vec[word_idx].global_vec;
        for (d = 0; d < dim; d++) context[d] += vec[d];
    }
    if (context_cnt == 0) return -1;
    for (d = 0; d < dim; d++) context[d] /= context_cnt;
    return 0;
}

void TestSCWS() {
    int i, tests_num, error;
    int word_a_idx, word_b_idx;
    char word_a[MAX_WORD_SIZE], word_b[MAX_WORD_SIZE];
    FILE* test_file = fopen(SCWS_FILE_PATH, "r");
    fscanf(test_file, "%d", &tests_num);
    double* self = (double*)malloc(tests_num * sizeof(double));
    double* standard = (double*)malloc(tests_num * sizeof(double));
    double* context_a = (double*)malloc(dim * sizeof(double));
    double* context_b = (double*)malloc(dim * sizeof(double));
    vec_list_t* sense_a;
    vec_list_t* sense_b;

    for (i = 0; i < tests_num; i++) {
        fscanf(test_file, "%s%s%lf", word_a, word_b, standard + i);
        word_a_idx = GetWordIdx(word_a);
        word_b_idx = GetWordIdx(word_b);
        error = 0;
        if (GetContext(test_file, context_a) == -1) error = 1;
        if (GetContext(test_file, context_b) == -1) error = 1;
        if (word_a_idx == NOT_IN_VOCAB || word_b_idx == NOT_IN_VOCAB || error == 1) {
            self[i] = -1;
            continue;
        }
        sense_a = ClosestSense(context_a, word_vec[word_a_idx].sense);
        sense_b = ClosestSense(context_b, word_vec[word_b_idx].sense);
        self[i] = CosinDist(sense_a->sense_vec, sense_b->sense_vec, dim);
    }
    printf("SCWS rating: %.6f\n", Spearman(self, standard, tests_num) * 100);
    free(self);
    free(standard);
    free(context_a);
    free(context_b);
    fclose(test_file);
}

void TestWordsim353() {
    int i, tests_num;
    int word_a_idx, word_b_idx;
    char word_a[MAX_WORD_SIZE], word_b[MAX_WORD_SIZE];
    FILE* test_file = fopen(WORDSIM353_FILE_PATH, "r");
    fscanf(test_file, "%d", &tests_num);
    double* self = (double*)malloc(tests_num * sizeof(double));
    double* standard = (double*)malloc(tests_num * sizeof(double));
    for (i = 0; i < tests_num; i++) {
        fscanf(test_file, "%s%s%lf", word_a, word_b, standard + i);
        word_a_idx = GetWordIdx(word_a);
        word_b_idx = GetWordIdx(word_b);
        if (word_a_idx == NOT_IN_VOCAB || word_b_idx == NOT_IN_VOCAB) self[i] = -1;
        else self[i] = CosinDist(word_vec[word_a_idx].global_vec, word_vec[word_b_idx].global_vec, dim);
    }
    printf("Wordsim353 rating: %.6f\n", Spearman(self, standard, tests_num) * 100);
    free(self);
    free(standard);
    fclose(test_file);
}

static int TestAnalogyQues(double* vec, int* ques, int depth) {
    vec_list_t* sense = word_vec[ques[depth]].sense;
    double* sense_vec;
    int result = 0, d;
    if (depth == 3) {
    } else {
        if (depth == 0) {
            for (; sense != NULL; sense = sense->next) {
                sense_vec = sense->sense_vec;
                for (d = 0; d < dim; d++) {
                    vec[d] = *sense_vec;
                    sense_vec++;
                }
                result |= TestAnalogyQues(vec, ques, depth + 1);
                if (result) return 1;
            }
        } else {
            for (; sense != NULL; sense = sense->next) {
                sense_vec = sense->sense_vec;
                for (d = 0; d < dim; d++) {
                    vec[d] += *sense_vec;
                    sense_vec++;
                }
                result |= TestAnalogyQues(vec, ques, depth + 1);
                sense_vec = sense->sense_vec;
                for (d = 0; d < dim; d++) {
                    vec[d] -= *sense_vec;
                    sense_vec++;
                }
                if (result) return 1;
            }
        }
    }
    return result;
}

static void TestAnalogySub(char* file_path) {
    FILE* test_file = fopen(file_path, "r");
    int i, j;
    int ques_num;
    int ques_cnt = 0;
    int correct_num = 0;
    int ques_idx[4], ques[4];
    int existed;
    char word[MAX_WORD_SIZE];
    double* vec = (double*)malloc(dim * sizeof(double));
    fscanf(test_file, "%d", &ques_num);
    for (i = 0; i < ques_num; i++) {
        existed = 1;
        for (j = 0; j < 4; j++) {
            fscanf(test_file, "%s", word);
            ques_idx[j] = GetWordIdx(word);
            if (ques_idx[j] == NOT_IN_VOCAB) existed = 0;
        }
        if (!existed) continue;
        else ques_cnt++;
        /* -a + b + c == d */
        ques[0] = ques_idx[0]; ques[1] = ques_idx[1];
        ques[2] = ques_idx[2]; ques[3] = ques_idx[3];
        if (TestAnalogyQues(vec, ques, 0)) {
            correct_num++;
            continue;
        }
        /* -b + a + d == c */
        ques[0] = ques_idx[1]; ques[1] = ques_idx[0];
        ques[2] = ques_idx[3]; ques[3] = ques_idx[2];
        if (TestAnalogyQues(vec, ques, 0)) {
            correct_num++;
            continue;
        }
        /* -d + b + c == a */
        ques[0] = ques_idx[3]; ques[1] = ques_idx[1];
        ques[2] = ques_idx[2]; ques[3] = ques_idx[0];
        if (TestAnalogyQues(vec, ques, 0)) {
            correct_num++;
            continue;
        }
        /* -c + a + d == b */
        ques[0] = ques_idx[2]; ques[1] = ques_idx[0];
        ques[0] = ques_idx[3]; ques[3] = ques_idx[1];
        if (TestAnalogyQues(vec, ques, 0)) {
            correct_num++;
            continue;
        }
    }
    free(vec);
}

void TestAnalogy() {
    TestAnalogySub(ANALOGY_SEMANTIC_FILE_PATH);
    TestAnalogySub(ANALOGY_SYNTACTIC_FILE_PATH);
}

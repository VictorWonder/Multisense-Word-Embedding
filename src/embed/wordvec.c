/********************************************************************************
 *  File Name       : wordvec.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-16 15:37
 *  Description     : 
********************************************************************************/

#include "common.h"
#include "vocab.h"
#include "embed.h"

int word_num = 0;                           /* word number i.e. vocabulary size */
word_vec_t* word_vec = NULL;                /* word vectors */

static int word_vec_init = 0;               /* initialization flag */

/* 
 * InitWordVec - initialize word vectors
 */
void InitWordVec() {
    /* if vocabulary is empty */
    if (word_num == 0)
        ReportError("Vocabulary is not existed!");

    /* if already initialized */
    if (word_vec_init) return;

    int i, k; /* iterators */
    vec_list_t* new_sense = NULL;
    word_vec = (word_vec_t*)malloc(word_num * sizeof(word_vec_t));
    for (i = 0; i < word_num; i++) {
        /* initialize gloabl vector */
        word_vec[i].global_vec = RandomNewVec(dim, &next_random);

        /* initialize context vector and sense vector */
        if (sense_model == MULTI_SENSE) {
            word_vec[i].sense_num = sense_num;
            word_vec[i].sense = NULL;

            /* vector list */
            for (k = 0; k < sense_num; k++) {
                new_sense = (vec_list_t*)malloc(sizeof(vec_list_t));
                new_sense->context_vec = (double*)calloc(dim, sizeof(double)); /* zero vector */
                new_sense->sense_vec = RandomNewVec(dim, &next_random);
                new_sense->cnt = 0;
                new_sense->word_idx = i;
                new_sense->sense_idx = k;
                new_sense->next = word_vec[i].sense;
                word_vec[i].sense = new_sense;
            }
        } else {
            word_vec[i].sense = NULL;
            word_vec[i].sense_num = 0;
        }
    }

    word_vec_init = 1;
}

void SaveWordVec(char* vector_path) {
    FILE* fout = fopen(vector_path, "w");
    if (fout == NULL)
        ReportError("Failed to save word vectors");

    int i, word_num = GetWordNum();
    fprintf(fout, "%d\n", word_num);
    for (i = 0; i < word_num; i++) {
        fprintf(fout, "%s %d\n", GetWord(i), word_vec[i].sense_num);
        WriteVec(fout, word_vec[i].global_vec, dim);
        if (sense_model != SINGLE_SENSE) {
            vec_list_t* sense = word_vec[i].sense;
            for (;sense != NULL; sense = sense->next) {
                WriteVec(fout, sense->context_vec, dim);
                WriteVec(fout, sense->sense_vec, dim);
            }
        }
    }
    fclose(fout);
}

void LoadWordVec(char* vector_path) {
    FILE* fin = fopen(vector_path, "r");
    if (fin == NULL)
        ReportError("Failed to load word vectors");

    int i, k, word_num;
    char word[1000];
    vec_list_t* new_sense;

    fscanf(fin, "%d%d", &word_num, &dim);
    word_vec = (word_vec_t*)malloc(word_num * sizeof(word_vec_t)); // TODO: build vocabulary and read single sense
    for (i = 0; i < word_num; i++) {
        //fscanf(fin, "%s", word);
        fscanf(fin, "%s%d", word, &word_vec[i].sense_num);
        word_vec[i].global_vec = (double*)malloc(dim * sizeof(double));
        ReadVec(fin, word_vec[i].global_vec, dim);

        word_vec[i].sense = NULL;
        for (k = 0; k < word_vec[i].sense_num; k++) {
            new_sense = (vec_list_t*)malloc(sizeof(vec_list_t));
            new_sense->context_vec = (double*)malloc(dim * sizeof(double));
            new_sense->sense_vec = (double*)malloc(dim * sizeof(double));
            ReadVec(fin, new_sense->context_vec, dim);
            ReadVec(fin, new_sense->sense_vec, dim);
            new_sense->cnt = 0;
            new_sense->word_idx = i;
            new_sense->sense_idx = k;
            new_sense->next = word_vec[i].sense;
            word_vec[i].sense = new_sense;
        }
    }
    word_vec_init = 1;
}


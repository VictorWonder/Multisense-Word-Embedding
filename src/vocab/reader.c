/********************************************************************************
 *  File Name       : reader.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-20 22:41
 *  Description     : 
********************************************************************************/

#include "common.h"
#include "vocab.h"

/* 
 * GetFileSize - return size of a file
 */
long long GetFileSize(char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL)
        ReportError("File is not existed!");
    fseek(file, 0, SEEK_END);
    long long file_size = ftell(file);
    fclose(file);
    return file_size;
}
/*
 * ReadWord - read a word from input file
 */
int ReadWord(char* word, FILE* fin) {
    int len = 0;
    char c;
    while (!feof(fin)) {
        c = fgetc(fin);
        if (c == 13) continue;
        if ((c == ' ') || (c == '\t') || (c == '\n')) {
            if (len > 0) {
                if (c == '\n') ungetc(c, fin);
                break;
            }
            if (c == '\n') return END_OF_SENTENCE;
            else continue;
        }
        word[len] = c;
        len++;
        if (len >= MAX_WORD_SIZE - 1) ReportError("Long word!");
    }
    word[len] = 0;
    return 0;
}

/* 
 * ReadWordIdx - Read a word from file and return its index in vocabulary
 */
int ReadWordIdx(FILE* fin) {
    char word[MAX_WORD_SIZE];
    if (ReadWord(word, fin) == END_OF_SENTENCE)
        return END_OF_SENTENCE;
    else return SearchHashTable(word);
}

/*
 * ReadSentence - Read a sentence and return its length
 */
int ReadSentence(FILE* fin, int* sen, int total_words, double subsample, unsigned long long* random_num) {
    int sentence_length = 0;
    int word;

    while (1) {
        /* read a word */
        word = ReadWordIdx(fin);
        if (feof(fin)) break;
        if (word == NOT_IN_VOCAB) continue;
        if (word == END_OF_SENTENCE) break;

        /* subsampling */
        if (subsample > 0) {
            int word_cnt = GetWordCnt(word);
            double prob = (sqrt(word_cnt / (subsample * total_words)) + 1) 
                * (subsample * total_words) / word_cnt;
            if (prob < RandomRealNum(random_num)) continue;
        }

        sen[sentence_length] = word;
        sentence_length++;
        if (sentence_length >= MAX_SENTENCE_LENGTH) break;
    }
    return sentence_length;
}

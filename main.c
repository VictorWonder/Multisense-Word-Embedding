/********************************************************************************
 *  File Name       : main.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-02 15:19
 *  Description     : 
********************************************************************************/

#include "cmg.h"

static int dim = 50;
static int window = 5;
static int min_count = 15;
static double alpha = 0.05;
static char train_file_path[MAX_STRING];
static char save_vocab_path[MAX_STRING];
static char load_vocab_path[MAX_STRING];

static int ArgPos(char*, int, char**);
static void Parse(int, char**);
static void BuildVocab();

// copy from google word2vec tool
static int ArgPos(char* str, int argc, char** argv) {
    register int i;
    for (i = 1; i < argc; i++) {
        if (!strcmp(str, argv[i])) {
            if (i == argc - 1) {
                printf("Argument missing for %s\n", str);
                exit(1);
            }
            return i;
        }
    }
    return -1;
}

static void Parse(int argc, char** argv) {
    register int i;
    train_file_path[0] = '\0';
    save_vocab_path[0] = '\0';
    load_vocab_path[0] = '\0';
    if ((i = ArgPos((char*)"-dim", argc, argv)) > 0) dim = atoi(argv[i + 1]);
    if ((i = ArgPos((char*)"-window", argc, argv)) > 0) window = atoi(argv[i + 1]);
    if ((i = ArgPos((char*)"-train", argc, argv)) > 0) strcpy(train_file_path, argv[i + 1]);
    if ((i = ArgPos((char*)"-min-count", argc, argv)) > 0) min_count = atoi(argv[i + 1]);
    if ((i = ArgPos((char*)"-alpha", argc, argv)) > 0) alpha = atof(argv[i + 1]);
    if ((i = ArgPos((char*)"-save-vocab", argc, argv)) > 0) strcpy(save_vocab_path, argv[i + 1]);
    if ((i = ArgPos((char*)"-load-vocab", argc, argv)) > 0) strcpy(load_vocab_path, argv[i + 1]);
}

static void BuildVocab() {
    if (load_vocab_path[0] == '\0')
        TrainVocab(train_file_path, save_vocab_path);
    else LoadVocab(load_vocab_path);
}

int main(int argc, char** argv) {
    Parse(argc, argv);
    BuildVocab();
    return 0;
}

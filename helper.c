/********************************************************************************
 *  File Name       : helper.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-16 15:44
 *  Description     : Some helper functions for this project
********************************************************************************/

#include "cmg.h"

inline unsigned long long NextRandomNum(unsigned long long rand) {
    return rand * (unsigned long long)998244353 + 1997;
}

inline double RandomRealNum(unsigned long long rand) {
    return (rand & 0xFFFF) / (double)0xFFFF;
}

inline void ReportError(char* error_msg) {
    fprintf(stderr, "ERROR: %s\n", error_msg);
    exit(-1);
}

inline void PrintUsage() {
}

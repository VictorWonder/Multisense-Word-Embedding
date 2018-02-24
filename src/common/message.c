/********************************************************************************
 *  File Name       : message.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-16 15:44
 *  Description     : Some helper functions for this project
********************************************************************************/

#include "common.h"

inline void ReportError(char* error_msg) {
    fprintf(stderr, "ERROR: %s\n", error_msg);
    exit(-1);
}

inline void WriteVec(FILE* fout, double* vec, int dim) {
    int d;
    double* ptr = vec;
    for (d = 0; d < dim; d++, ptr++) 
        fprintf(fout, "%.6f ", *ptr);
    fprintf(fout, "\n");
}

inline void ReadVec(FILE* fin, double* vec, int dim) {
    int d;
    double* ptr = vec;
    for (d = 0; d < dim; d++, ptr++)
        fscanf(fin, "%lf", ptr);
}

inline void WriteMatrix(FILE* fout, double* matrix, int row, int cloumn) {
    int i, j;
    double* ptr = matrix;
    for (i = 0; i < row; i++) {
        for (j = 0; j < cloumn; j++, ptr++) 
            fprintf(fout, "%.6f ", *ptr);
        fprintf(fout, "\n");
    }
}

inline void ReadMatrix(FILE* fin, double* matrix, int row, int cloumn) {
    int i, j;
    double* ptr = matrix;
    for (i = 0; i < row; i++)
        for (j = 0; j < cloumn; j++, ptr++)
            fscanf(fin, "%lf", ptr);
}

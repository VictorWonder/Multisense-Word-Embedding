/********************************************************************************
 *  File Name       : math.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-21 09:42
 *  Description     : 
********************************************************************************/

#include "common.h"

/*
 * NextRandomNum - return next pseudo random number
 */
inline void NextRandomNum(unsigned long long* rand) {
    //*rand = *rand * (unsigned long long)998244353 + 1997;
    *rand = *rand * (unsigned long long)25214903917 + 11;
}

/*
 * RandomRealNum - return a double float pseudo random number
 */
inline double RandomRealNum(unsigned long long* rand) {
    NextRandomNum(rand);
    return (*rand & 0xFFFF) / (double)65536;
}

/*
 * RandomNewVec - return a initialized new vector
 */
inline double* RandomNewVec(int dim, unsigned long long* rand) {
    int d;
    double* vec = (double*)malloc(dim * sizeof(double));
    for (d = 0; d < dim; d++) 
        vec[d] = RandomRealNum(rand) - 0.5;
    return vec;
}

inline double* RandomNewMat(int n, int m, unsigned long long* rand) {
    int d;
    double* mat = (double*)malloc(n * m * sizeof(double));
    for (d = 0; d < n * m; d++)
        mat[d] = RandomRealNum(rand) - 0.5;
    return mat;
}

/*
 * Dot - dot product of two vectors
 */
inline double Dot(double* vec_a, double* vec_b, int dim) {
    int d;
    double sum = 0;
    for (d = 0; d < dim; d++)
        sum += vec_a[d] * vec_b[d];
    return sum;
}

/*
 * VectorLength - return length of a vector
 */
inline double VectorLength(double* vec, int dim) {
    int d;
    double sum = 0;
    for (d = 0; d < dim; d++) 
        sum += vec[d] * vec[d];
    return sqrt(sum);
}

/*
 * EuclidDist - return the euclid dist between two vectors
 */
inline double EuclidDist(double* vec_a, double* vec_b, int dim) {
    int d;
    double sum = 0;
    for (d = 0; d < dim; d++) 
        sum += (vec_a[d] - vec_b[d]) * (vec_a[d] - vec_b[d]);
    return sqrt(sum);
}

/*
 * CosinDist - return the cosin dist between two vectors
 */
inline double CosinDist(double* vec_a, double* vec_b, int dim) {
    double sum = Dot(vec_a, vec_b, dim);
    return sum / (VectorLength(vec_a, dim) * VectorLength(vec_b, dim));
}

/*
 * MatDotVec - return the result of a n * m matrix multiply a m-d vector
 */
inline void MatDotVec(double* mat, double* vec, int n, int m) {
    int i, j;
    double* result = (double*)malloc(m * sizeof(double));
    for (i = 0; i < n; i++) {
        result[i] = 0;
        for (j = 0; j < m; j++)
            result[i] += mat[i * m + j] * vec[j];
    }
    for (i = 0; i < m; i++) vec[i] = result[i];
    free(result);
}

/* 
 * MatDotMat - return the result of a n * m matrix multiply a m * l matrix
 */
inline double* MatDotMat(double* mat_a, double* mat_b, int n, int m, int l) {
    int i, j, k, result_size = n * l;
    double* result = (double*)malloc(result_size * sizeof(double));
    double* result_ptr = result;
    double* mat_ptr_a = mat_a;
    double* mat_ptr_b = mat_b;
    for (i = 0; i < n; i++) {           /* i-th row of result matrix */
        for (k = 0; k < l; k++) {       /* k-th cloumn of result matrix */
            *result_ptr = 0;
            mat_ptr_a = mat_a + i * m;
            mat_ptr_b = mat_b + k;
            for (j = 0; j < m; j++) {
                *result_ptr += (*mat_ptr_a) * (*mat_ptr_b);
                mat_ptr_a++;
                mat_ptr_b += l;
            }
            result_ptr++;
        }
    }
    return result;
}

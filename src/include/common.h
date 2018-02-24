/********************************************************************************
 *  File Name       : common.h
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-20 22:59
 *  Description     : Useful helper function
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

extern unsigned long long next_random;

// message.c
extern void ReportError(char*);
extern void PrintUsage();
extern void WriteVec(FILE*, double*, int);
extern void ReadVec(FILE*, double*, int);
extern void WriteMatrix(FILE*, double*, int, int);
extern void ReadMatrix(FILE*, double*, int, int);

// math.c
extern void NextRandomNum(unsigned long long*);
extern double RandomRealNum(unsigned long long*);
extern double* RandomNewVec(int, unsigned long long*);
extern double* RandomNewMat(int, int, unsigned long long*);
extern double Dot(double*, double*, int);
extern double VectorLength(double*, int);
extern double EuclidDist(double*, double*, int);
extern double CosinDist(double*, double*, int);
extern void MatDotVec(double*, double*, int, int);
extern double* MatDotMat(double*, double*, int, int, int);

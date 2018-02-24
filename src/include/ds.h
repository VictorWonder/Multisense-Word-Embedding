/********************************************************************************
 *  File Name       : ds.h
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-26 13:27
 *  Description     : Header file for data structures
********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEAP_SIZE 100
#define MAX_NEIGHBOR_NUM MAX_HEAP_SIZE

typedef struct {
    void* ptr;
    double dist;
} heap_node_t;

extern double** tree_node;

/* kdtree.c */
extern void BuildKDTree(int, int, int, int);
extern int KNearestNeighbor(double**, int, double*, int, int);

/* heap.c */
extern void PushHeap(void*, double);
extern heap_node_t* HeapTop();
extern void PopHeap();
extern void ClearHeap();
extern int GetHeapSize();

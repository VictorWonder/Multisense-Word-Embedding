/********************************************************************************
 *  File Name       : kdtree.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-09 15:46
 *  Description     : 
********************************************************************************/

#include "common.h"
#include "ds.h"

double** tree_node;

static double* k_nearest_vec[MAX_NEIGHBOR_NUM];

static void NthElements(int, int, int, int);
static void Query(int, int, double*, int, int, int);

static void NthElements(int l, int r, int n, int pos) {
    if (l >= r) return;
    double* pivot = tree_node[l];
    int i = l, j = r;
    while (i < j) {
        while (i < j && tree_node[j][pos] >= pivot[pos]) j--;
        if (i < j) tree_node[i] = tree_node[j];
        while (i < j && tree_node[i][pos] <= pivot[pos]) i++;
        if (i < j) tree_node[j] = tree_node[i];
    }
    tree_node[i] = pivot;
    if (i >= n) NthElements(l, i, n, pos);
    else NthElements(i + 1, r, n, pos);
}

static void Query(int l, int r, double* vec, int k, int pos, int dim) {
    if (l > r) return;
    int mid = (l + r) >> 1;
    double dist = EuclidDist(vec, tree_node[mid], dim);
    if (GetHeapSize() == k && HeapTop()->dist > dist) PopHeap();
    if (GetHeapSize() < k) PushHeap(tree_node[mid], dist);
    double t = vec[pos] - tree_node[mid][pos];
    if (t <= 0) {
        Query(l, mid - 1, vec, k, (pos + 1) % dim, dim);
        if (HeapTop()->dist > t * t || GetHeapSize() < k)
            Query(mid + 1, r, vec, k, (pos + 1) % dim, dim);
    } else {
        Query(mid + 1, r, vec, k, (pos + 1) % dim, dim);
        if (HeapTop()->dist > t * t || GetHeapSize() < k)
            Query(l, mid - 1, vec, k, (pos + 1) % dim, dim);
    }
}

void BuildKDTree(int l, int r, int pos, int dim) {
    if (l > r) return;
    int mid = (l + r) >> 1;
    NthElements(l, r, mid, pos);
    BuildKDTree(l, mid - 1, (pos + 1) % dim, dim);
    BuildKDTree(mid + 1, r, (pos + 1) % dim, dim);
}

int KNearestNeighbor(double** result, int total_senses, double* vec, int k, int dim) {
    int top_k = 0;
    ClearHeap();
    Query(0, total_senses - 1, vec, k, 0, dim);
    while (GetHeapSize() != 0) {
        result[top_k] = HeapTop()->ptr;
        PopHeap();
        top_k++;
    }
    return top_k;
}

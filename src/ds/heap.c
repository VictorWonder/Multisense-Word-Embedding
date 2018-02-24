/********************************************************************************
 *  File Name       : heap.c
 *  Author          : Xihao Wang
 *  Email           : victorwonder@pku.edu.cn
 *  Created Time    : 2018-01-26 14:48
 *  Description     : 
********************************************************************************/

#include "common.h"
#include "ds.h"

static int heap_size = 0;
static heap_node_t heap[MAX_HEAP_SIZE];
static heap_node_t* heap_top = &heap[1];

void PushHeap(void* ptr, double dist) {
    heap_size++;
    heap[heap_size].ptr = ptr;
    heap[heap_size].dist = dist;
    
    int i = heap_size, j = i >> 1;
    while (i != 1) {
        if (heap[i].dist < heap[j].dist) {
            heap_node_t t = heap[i];
            heap[i] = heap[j];
            heap[j] = t;
        } else break;
        i = j;
        j >>= 1;
    }
}

heap_node_t* HeapTop() {
    return heap_top;
}

void PopHeap() {
    int i = 1, j;
    heap[1] = heap[heap_size];
    heap_size--;
    
    while ((i << 1) <= heap_size) {
        if ((i << 1 | 1) > heap_size) j = i << 1;
        else if (heap[i << 1].dist < heap[i << 1 | 1].dist) j = i << 1;
        else j = i << 1 | 1;
        if (heap[i].dist < heap[j].dist) break;
        else {
            heap_node_t t = heap[i];
            heap[i] = heap[j];
            heap[j] = t;
            i = j;
        }
    }
}

void ClearHeap() {
    heap_size = 0;
}

int GetHeapSize() {
    return heap_size;
}

#include "heap.h"
#include <stdlib.h>

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}

void reheap(Process *p, int *heap, int size, int i) {
    int smallest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < size && p[heap[l]].burst < p[heap[smallest]].burst) smallest = l;
    if (r < size && p[heap[r]].burst < p[heap[smallest]].burst) smallest = r;
    if (smallest != i) {
        swap(&heap[i], &heap[smallest]);
        reheap(p, heap, size, smallest);
    }
}

void pushHeap(Process *p, int *heap, int *size, int idx) {
    heap[(*size)++] = idx;
    int i = *size - 1;
    while (i && p[heap[i]].burst < p[heap[(i - 1) / 2]].burst) {
        swap(&heap[i], &heap[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

int popHeap(Process *p, int *heap, int *size) {
    int root = heap[0];
    heap[0] = heap[--(*size)];
    reheap(p, heap, *size, 0);
    return root;
}
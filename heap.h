#pragma once
#include "pclass.h"

void swap(int *a, int *b);
void reheap(Process *p, int *heap, int size, int i);
void pushHeap(Process *p, int *heap, int *size, int idx);
int popHeap(Process *p, int *heap, int *size);
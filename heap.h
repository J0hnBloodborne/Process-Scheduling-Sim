#pragma once
#include "pclass.h"

void swap(int *a, int *b);
void heapify(Process *p, int *heap, int size, int i);
void push_heap(Process *p, int *heap, int *size, int idx);
int pop_heap(Process *p, int *heap, int *size);
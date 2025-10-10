#pragma once
#include "pclass.h"

int cmp_arrival(Process *a, Process *b);
int cmp_burst(Process *a, Process *b);
void swap(Process *a, Process *b);
int partition(Process arr[], int low, int high, int (*cmp)(Process*, Process*));
void quicksort(Process arr[], int low, int high, int (*cmp)(Process*, Process*));
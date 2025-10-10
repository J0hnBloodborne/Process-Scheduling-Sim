#pragma once
#include "pclass.h"

void swapProcess(Process *a, Process *b);
int partition(Process arr[], int low, int high);
void quicksort(Process arr[], int low, int high);
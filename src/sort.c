#include "sort.h"

void swapProcess(Process *a, Process *b) {
    Process temp = *a;
    *a = *b;
    *b = temp;
}

int partition(Process arr[], int low, int high) {
    Process *pivot = &arr[high];
    int i = low - 1;
    for (int j = low; j < high; j++) {
        if (arr[j].arrival - pivot->arrival < 0) {
            i++;
            swapProcess(&arr[i], &arr[j]);
        }
    }
    swapProcess(&arr[i + 1], &arr[high]);
    return i + 1;
}

void quicksort(Process arr[], int low, int high) {
    if (low < high) {
        int pi = partition(arr, low, high);
        quicksort(arr, low, pi - 1);
        quicksort(arr, pi + 1, high);
    }
}
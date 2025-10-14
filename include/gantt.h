#pragma once
#include <windows.h>

typedef struct Slice {
    int pid;
    int start;
    int end;
} Slice;

void ShowGanttWindow(const char* title,
                     const Slice* fcfs, int n_fcfs,
                     const Slice* sjf,  int n_sjf,
                     const Slice* rr,   int n_rr,
                     int makespan);

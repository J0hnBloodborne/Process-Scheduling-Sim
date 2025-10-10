#pragma once

typedef struct {
    int pid;
    int burst;
    int arrival;
    int waiting;
    int turnaround;
    int completion;
    int remaining;
} Process;
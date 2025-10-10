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

typedef struct {
    double avg_waiting;
    double avg_turnaround;
} SchedResult;
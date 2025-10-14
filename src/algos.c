#include "../include/algos.h"
#include "../include/heap.h"
#include "../include/pclass.h"
#include <stdbool.h>
#include <stdlib.h>

SchedResult fcfs(Process *p, int n) {
    quicksort(p, 0, n - 1);
    int current_time = 0;
    int total_waiting = 0, total_turnaround = 0;
    for (int i = 0; i < n; i++) {
        if (current_time < p[i].arrival) current_time = p[i].arrival;
        current_time += p[i].burst;
        p[i].completion = current_time;
        p[i].turnaround = p[i].completion - p[i].arrival;
    p[i].waiting = p[i].turnaround - p[i].burst;
    p[i].response = p[i].waiting;
        total_waiting += p[i].waiting;
        total_turnaround += p[i].turnaround;
    }
    SchedResult r = { total_waiting / (double)n, total_turnaround / (double)n };
    return r;
}

SchedResult sjf(Process *p, int n) {
    int current_time = 0, done = 0;
    int total_waiting = 0, total_turnaround = 0;

    int *heap = malloc(n * sizeof(int));
    int heap_size = 0;
    bool *added = calloc(n, sizeof(bool));

    int earliest = INT_MAX;
    for (int i = 0; i < n; i++) if (p[i].arrival < earliest) earliest = p[i].arrival;
    current_time = earliest;
    for (int i = 0; i < n; i++) if (p[i].arrival == earliest) { 
        pushHeap(p, heap, &heap_size, i); 
        added[i] = true; 
    }

    while (done < n) {
        if (heap_size == 0) {
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++)
                if (!added[i] && p[i].arrival < next_arrival)
                    next_arrival = p[i].arrival;
            current_time = next_arrival;
            for (int i = 0; i < n; i++)
                if (!added[i] && p[i].arrival == current_time) {
                    pushHeap(p, heap, &heap_size, i);
                    added[i] = true;
                }
            continue;
        }

        int idx = popHeap(p, heap, &heap_size);
        p[idx].completion = current_time + p[idx].burst;
        p[idx].turnaround = p[idx].completion - p[idx].arrival;
    p[idx].waiting = current_time - p[idx].arrival;
    p[idx].response = p[idx].waiting;
        p[idx].remaining = 0;
        current_time = p[idx].completion;
        done++;

        total_waiting += p[idx].waiting;
        total_turnaround += p[idx].turnaround;

        for (int i = 0; i < n; i++)
            if (!added[i] && p[i].arrival <= current_time) {
                pushHeap(p, heap, &heap_size, i);
                added[i] = true;
            }
    }
    free(heap);
    free(added);

    SchedResult r = { total_waiting / (double)n, total_turnaround / (double)n };
    return r;
}

SchedResult rr(Process *p, int n, int q) {
    int current_time = 0, done = 0;
    int total_waiting = 0, total_turnaround = 0;

    int *queue = malloc(n * n * sizeof(int));
    int front = 0, rear = 0;
    bool *in_queue = calloc(n, sizeof(bool));

    if (rear == 0) {
        int earliest = INT_MAX;
        for (int i = 0; i < n; i++) if (p[i].arrival < earliest) earliest = p[i].arrival;
        current_time = earliest;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival == earliest) {
                queue[rear++] = i;
                in_queue[i] = true;
            }
        }
    }

    while (front < rear) {
        int i = queue[front++];
        if (p[i].remaining > 0 && p[i].arrival <= current_time) {
            int slice = (p[i].remaining > q) ? q : p[i].remaining;
            if (p[i].remaining == p[i].burst) {
                p[i].response = current_time - p[i].arrival;
            }
            p[i].remaining -= slice;
            current_time += slice;
            for (int j = 0; j < n; j++) {
                if (!in_queue[j] && p[j].arrival <= current_time) {
                    queue[rear++] = j;
                    in_queue[j] = true;
                }
            }
            if (p[i].remaining == 0) {
                p[i].completion = current_time;
                p[i].turnaround = p[i].completion - p[i].arrival;
                p[i].waiting = p[i].turnaround - p[i].burst;
                total_waiting += p[i].waiting;
                total_turnaround += p[i].turnaround;
                done++;
            } else {
                queue[rear++] = i;
            }
        } else if (p[i].remaining > 0) {
            current_time = p[i].arrival;
            queue[rear++] = i;
        }
    }
    free(queue);
    free(in_queue);

    SchedResult r = { total_waiting / (double)n, total_turnaround / (double)n };
    return r;
}

SchedResult rr_ctx(Process *p, int n, int q, int *ctx_switches) {
    int current_time = 0, done = 0;
    int total_waiting = 0, total_turnaround = 0;
    int switches = 0;

    int *queue = malloc(n * n * sizeof(int));
    int front = 0, rear = 0;
    bool *in_queue = calloc(n, sizeof(bool));

    int last_pid = -1;

    if (rear == 0) {
        int earliest = INT_MAX;
        for (int i = 0; i < n; i++) if (p[i].arrival < earliest) earliest = p[i].arrival;
        current_time = earliest;
        for (int i = 0; i < n; i++) {
            if (p[i].arrival == earliest) {
                queue[rear++] = i;
                in_queue[i] = true;
            }
        }
    }

    while (front < rear) {
        int i = queue[front++];
        if (p[i].remaining > 0 && p[i].arrival <= current_time) {
            if (last_pid != -1 && last_pid != p[i].pid) switches++;
            last_pid = p[i].pid;

            int slice = (p[i].remaining > q) ? q : p[i].remaining;
            if (p[i].remaining == p[i].burst) {
                p[i].response = current_time - p[i].arrival;
            }
            p[i].remaining -= slice;
            current_time += slice;
            for (int j = 0; j < n; j++) {
                if (!in_queue[j] && p[j].arrival <= current_time) {
                    queue[rear++] = j;
                    in_queue[j] = true;
                }
            }
            if (p[i].remaining == 0) {
                p[i].completion = current_time;
                p[i].turnaround = p[i].completion - p[i].arrival;
                p[i].waiting = p[i].turnaround - p[i].burst;
                total_waiting += p[i].waiting;
                total_turnaround += p[i].turnaround;
                done++;
            } else {
                queue[rear++] = i;
            }
        } else if (p[i].remaining > 0) {
            current_time = p[i].arrival;
            queue[rear++] = i;
        }
    }
    free(queue);
    free(in_queue);

    if (ctx_switches) *ctx_switches = switches;
    SchedResult r = { total_waiting / (double)n, total_turnaround / (double)n };
    return r;
}
#include "pclass.h"
#include "reader.h"
#include "algos.h"
#include <stdlib.h>
#include <stdio.h>

int main() {
    char* filename = fileExplorerDiag();
    while (!filename) {
        printf("No file selected or dialog was canceled. Please try again.\n");
        filename = fileExplorerDiag();
    }
    printf("Selected file: %s\n", filename);
    int n = countLines(filename);
    Process* processes = malloc(n * sizeof(Process));
    int loaded = loadCSV(filename, processes);

    printf("Loaded %d processes from the file.\n", loaded);
    for (int i = 0; i < loaded; i++) printf("Process ID: %d, Burst Time: %d, Arrival Time: %d\n", processes[i].pid, processes[i].burst, processes[i].arrival);

    Process* proc_fcfs = malloc(loaded * sizeof(Process));
    Process* proc_sjf = malloc(loaded * sizeof(Process));
    Process* proc_rr = malloc(loaded * sizeof(Process));
    for (int i = 0; i < loaded; i++) {
        proc_fcfs[i] = processes[i];
        proc_sjf[i] = processes[i];
        proc_rr[i] = processes[i];
        proc_rr[i].remaining = processes[i].burst;
    }

    int quantum;
    printf("Quantum for RR? ");
    scanf("%d", &quantum);

    SchedResult res_fcfs = fcfs(proc_fcfs, loaded);
    SchedResult res_sjf = sjf(proc_sjf, loaded);
    SchedResult res_rr = rr(proc_rr, loaded, quantum);

    printf("\nResults:\n");
    printf("FCFS: Avg Waiting = %.2f, Avg Turnaround = %.2f\n", res_fcfs.avg_waiting, res_fcfs.avg_turnaround);
    printf("SJF:  Avg Waiting = %.2f, Avg Turnaround = %.2f\n", res_sjf.avg_waiting, res_sjf.avg_turnaround);
    printf("RR:   Avg Waiting = %.2f, Avg Turnaround = %.2f (Quantum: %d)\n", res_rr.avg_waiting, res_rr.avg_turnaround, quantum);

    free(processes);
    free(proc_fcfs);
    free(proc_sjf);
    free(proc_rr);
}
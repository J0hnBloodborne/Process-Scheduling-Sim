#include"pclass.h"
#include"reader.h"
#include"algos.h"
#include<stdlib.h>
#include<stdio.h>

int main(){
    char* filename = fileExplorerDiag();
    while (!filename) {
        printf("No file selected or dialog was canceled. Please try again.\n");
        filename = fileExplorerDiag();
    }
    printf("Selected file: %s\n", filename);
    int n = countLines(filename);
    Process* processes = malloc(n * sizeof(Process));
    int loaded = loadCSV(filename, processes);

    //Debugging output to verify loaded processes
    printf("Loaded %d processes from the file.\n", loaded);
    for (int i = 0; i < loaded; i++) {
        printf("Process ID: %d, Burst Time: %d, Arrival Time: %d\n", processes[i].pid, processes[i].burst, processes[i].arrival);
    }
    
    free(processes);
    return 0;
}
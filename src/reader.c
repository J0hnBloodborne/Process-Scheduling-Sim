#include "reader.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <commdlg.h>

char* fileExplorerDiag() {
    static char filename[MAX_PATH] = "";
    OPENFILENAME ofn = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "CSV Files\0*.csv\0All Files\0*.*\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST;
    ofn.lpstrTitle = "Select your CSV file";
    if (GetOpenFileName(&ofn)) return filename;
    return NULL;
}

int countLines(char *filename) {
    FILE *file = fopen(filename, "r");
    int l = 0;
    while (!feof(file)) if (fgetc(file) == '\n') l++;
    fclose(file);
    return l;
}

int loadCSV(char *filename, Process *p){
    FILE *file = fopen(filename, "r");
    char line[32];
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        p[i].pid = atoi(strtok(line, ","));
        p[i].burst = atoi(strtok(NULL, ","));
        p[i].arrival = atoi(strtok(NULL, ","));
        p[i].waiting = 0;
        p[i].turnaround = 0;
        p[i].remaining = p[i].burst;
        p[i].completion = 0;
        i++;
    }
    fclose(file);
    return i;
}
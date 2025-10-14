#include "../include/reader.h"
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
    int l = 0, c, last_was_nl = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') { l++; last_was_nl = 1; }
        else last_was_nl = 0;
    }
    fclose(file);
    return l + (!last_was_nl ? 1 : 0);
}

int loadCSV(char *filename, Process *p){
    FILE *file = fopen(filename, "r");
    if (!file) return 0;
    char line[64];
    int i = 0;
    while (fgets(line, sizeof(line), file)) {
        char *tok = strtok(line, ",");
        if (!tok) break;
        p[i].pid = atoi(tok);

        tok = strtok(NULL, ",");
        if (!tok) break;
        p[i].arrival = atoi(tok);

        tok = strtok(NULL, ",\r\n");
        if (!tok) break;
        p[i].burst = atoi(tok);

        p[i].waiting = 0;
        p[i].turnaround = 0;
        p[i].remaining = p[i].burst;
        p[i].completion = 0;
        i++;
    }
    fclose(file);
    return i;
}
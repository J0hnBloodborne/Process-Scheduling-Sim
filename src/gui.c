#include "gui.h"

#define ID_BTN_OPEN 1
#define ID_BTN_START 2
#define ID_BTN_EXIT 3
#define ID_STATIC_PATH 1002

static char selectedFile[MAX_PATH] = {0};
static int quantumInput = 0;
static HWND hStaticPath = NULL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

const char* GetSelectedFile() { return selectedFile; }
int GetQuantumInput() { return quantumInput; }

void ShowMainWindow(HINSTANCE hInstance) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "ProcSchedSim";
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, wc.lpszClassName, "Process Scheduling Simulator",
        WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 400, 200,
        NULL, NULL, hInstance, NULL);
    CreateWindow("BUTTON", "Open input file", WS_VISIBLE | WS_CHILD,
        20, 30, 120, 30, hwnd, (HMENU)ID_BTN_OPEN, hInstance, NULL);
    CreateWindow("BUTTON", "Start Test", WS_VISIBLE | WS_CHILD,
        150, 30, 120, 30, hwnd, (HMENU)ID_BTN_START, hInstance, NULL);
    CreateWindow("BUTTON", "Exit", WS_VISIBLE | WS_CHILD,
        280, 30, 80, 30, hwnd, (HMENU)ID_BTN_EXIT, hInstance, NULL);

    hStaticPath = CreateWindow("STATIC", "No file selected.", WS_VISIBLE | WS_CHILD | SS_LEFT,
        20, 70, 340, 20, hwnd, (HMENU)ID_STATIC_PATH, hInstance, NULL);

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void OpenInputFileDialog(HWND hwnd) {
    OPENFILENAME ofn = {0};
    char fileName[MAX_PATH] = {0};
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = fileName;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "CSV Files\0*.csv\0All Files\0*.*\0";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = "Select an input CSV file";
    if (GetOpenFileName(&ofn)) {
        strcpy(selectedFile, fileName);
        MessageBox(hwnd, selectedFile, "Selected File", MB_OK);
        if (hStaticPath) {
            SetWindowText(hStaticPath, selectedFile);
        }
    }
}

static char quantumInputBuffer[16] = {0};

INT_PTR CALLBACK QuantumDlgProc(HWND dHwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(w) == IDOK) {
                GetDlgItemText(dHwnd, 1001, quantumInputBuffer, 15);
                quantumInput = atoi(quantumInputBuffer);
                EndDialog(dHwnd, IDOK);
                return 1;
            }
            break;
        case WM_CLOSE:
            EndDialog(dHwnd, 0);
            return 1;
    }
    return 0;
}

void AskQuantumDialog(HWND hwnd) {
    quantumInputBuffer[0] = '\0';
    if (DialogBoxParam(NULL, MAKEINTRESOURCE(101), hwnd, QuantumDlgProc, 0) == IDOK) {
        char buf[32];
        sprintf(buf, "Quantum: %d", quantumInput);
        MessageBox(hwnd, buf, "Quantum Set", MB_OK);
    }
}

void RunScheduling(HWND hwnd) {
    if (selectedFile[0] == '\0') {
        MessageBox(hwnd, "No input file selected!", "Error", MB_ICONERROR);
        return;
    }
    if (quantumInput <= 0) {
        MessageBox(hwnd, "Quantum must be set and > 0!", "Error", MB_ICONERROR);
        return;
    }

    int n = countLines(selectedFile);
    Process* processes = malloc(n * sizeof(Process));
    int loaded = loadCSV(selectedFile, processes);
    printf("Loaded %d processes from the file.\n", loaded);
    for (int i = 0; i < loaded; i++) {
        printf("Process ID: %d, Burst Time: %d, Arrival Time: %d\n", processes[i].pid, processes[i].burst, processes[i].arrival);
    }

    if (loaded <= 0) {
        MessageBox(hwnd, "Failed to load processes!", "Error", MB_ICONERROR);
        free(processes);
        return;
    }

    Process* proc_fcfs = malloc(loaded * sizeof(Process));
    Process* proc_sjf  = malloc(loaded * sizeof(Process));
    Process* proc_rr   = malloc(loaded * sizeof(Process));
    for (int i = 0; i < loaded; i++) {
        proc_fcfs[i] = processes[i];
        proc_sjf[i]  = processes[i];
        proc_rr[i]   = processes[i];
        proc_rr[i].remaining = processes[i].burst;
    }

    SchedResult res_fcfs = fcfs(proc_fcfs, loaded);
    SchedResult res_sjf  = sjf(proc_sjf, loaded);
    SchedResult res_rr   = rr(proc_rr, loaded, quantumInput);

    ShowGraphWindow(loaded, res_fcfs.avg_waiting, res_sjf.avg_waiting, res_rr.avg_waiting,
                res_fcfs.avg_turnaround, res_sjf.avg_turnaround, res_rr.avg_turnaround);

    // printf("\nResults:\n");
    // printf("FCFS: Avg Waiting = %.2f, Avg Turnaround = %.2f\n", res_fcfs.avg_waiting, res_fcfs.avg_turnaround);
    // printf("SJF:  Avg Waiting = %.2f, Avg Turnaround = %.2f\n", res_sjf.avg_waiting, res_sjf.avg_turnaround);
    // printf("RR:   Avg Waiting = %.2f, Avg Turnaround = %.2f (Quantum: %d)\n", res_rr.avg_waiting, res_rr.avg_turnaround, quantumInput);

    free(processes);
    free(proc_fcfs);
    free(proc_sjf);
    free(proc_rr);

    // MessageBox(hwnd, "Scheduling test results printed to terminal.", "Done", MB_OK);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_COMMAND:
            switch (LOWORD(wParam)) {
                case ID_BTN_OPEN:
                    OpenInputFileDialog(hwnd);
                    break;
                case ID_BTN_START:
                    AskQuantumDialog(hwnd);
                    RunScheduling(hwnd);
                    break;
                case ID_BTN_EXIT:
                    PostQuitMessage(0);
                    break;
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}
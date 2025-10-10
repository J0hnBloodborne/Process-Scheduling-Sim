#include "gui.h"

#define ID_BTN_OPEN 1
#define ID_BTN_START 2
#define ID_BTN_EXIT 3

static char selectedFile[MAX_PATH] = {0};
static int quantumInput = 0;

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
    ofn.lpstrFilter = "All Files\0*.*\0";
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    if (GetOpenFileName(&ofn)) {
        strcpy(selectedFile, fileName);
        MessageBox(hwnd, selectedFile, "Selected File", MB_OK);
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
        // quantumInput is now set
        char buf[32];
        sprintf(buf, "Quantum: %d", quantumInput);
        MessageBox(hwnd, buf, "Quantum Set", MB_OK);
    }
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
                    // TODO: call your scheduling/graph code here
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
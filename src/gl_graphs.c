#include "../include/gl_graphs.h"
#include <windows.h>
#include <stdio.h>

static double bars[3];
static const char* barlabels[3] = { "FCFS", "SJF", "RR" };
static COLORREF barcolors[3] = { RGB(255,0,127), RGB(127,255,0), RGB(0,127,255) };
static char graphtitle[64];
static char graphinfo[128]; // New: extra info line

#define TITLE_PAD 100

LRESULT CALLBACK GraphWndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);

void DrawBarGraph(HWND hwnd, HDC hdc) {
    RECT client;
    GetClientRect(hwnd, &client);
    int width = client.right - client.left;
    int height = client.bottom - client.top;
    int margin = 40;
    int barw = 80;
    int gap = 30;
    int top_y = TITLE_PAD + 30;
    int base_y = height - margin - 40;

    double maxv = bars[0];
    for (int i = 1; i < 3; i++) if (bars[i] > maxv) maxv = bars[i];

    // Title
    SetBkMode(hdc, TRANSPARENT);
    HFONT hFont = CreateFontA(22,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Arial");
    SelectObject(hdc, hFont);
    TextOutA(hdc, width/2-120, 20, graphtitle, lstrlenA(graphtitle));
    DeleteObject(hFont);

    // Extra info (num processes, etc)
    HFONT infoFont = CreateFontA(15,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Arial");
    SelectObject(hdc, infoFont);
    TextOutA(hdc, width/2-120, 55, graphinfo, lstrlenA(graphinfo));
    DeleteObject(infoFont);

    // Bars, values, labels
    int bx = margin;
    HFONT barFont = CreateFontA(16,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Arial");
    SelectObject(hdc, barFont);
    int bar_max_height = base_y - top_y;
    for (int i = 0; i < 3; i++) {
        int bh = (int)((bars[i]/maxv)*(bar_max_height));
        int bar_top = base_y - bh;

        HBRUSH brush = CreateSolidBrush(barcolors[i]);
        RECT r = { bx, bar_top, bx+barw, base_y };
        FillRect(hdc, &r, brush);
        DeleteObject(brush);

        char valbuf[128];
        sprintf(valbuf, "%.2f", bars[i]);
        TextOutA(hdc, bx+(barw/2)-20, bar_top-24, valbuf, lstrlenA(valbuf));

        TextOutA(hdc, bx+(barw/2)-20, base_y+8, barlabels[i], lstrlenA(barlabels[i]));

        bx += barw + gap;
    }
    DeleteObject(barFont);
}

LRESULT CALLBACK GraphWndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            DrawBarGraph(hwnd, hdc);
            EndPaint(hwnd, &ps);
        } break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, msg, w, l);
    }
    return 0;
}

void ShowGraph(const char* title, const char* info, double v0, double v1, double v2) {
    bars[0]=v0; bars[1]=v1; bars[2]=v2;
    snprintf(graphtitle, sizeof(graphtitle), "%s", title);
    snprintf(graphinfo, sizeof(graphinfo), "%s", info);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = GraphWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = title;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, title,
        WS_OVERLAPPEDWINDOW^WS_THICKFRAME, CW_USEDEFAULT, CW_USEDEFAULT, 420, 540,
        NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
}

void ShowGraphWindow(
    int num_processes,
    double fcfs_wait, double sjf_wait, double rr_wait,
    double fcfs_turn, double sjf_turn, double rr_turn
) {
    char info[512];
    snprintf(info, sizeof(info), "Total processes: %d", num_processes);
    ShowGraph("Average Waiting Time", info, fcfs_wait, sjf_wait, rr_wait);

    snprintf(info, sizeof(info), "Total processes: %d", num_processes);
    ShowGraph("Average Turnaround Time", info, fcfs_turn, sjf_turn, rr_turn);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
        DispatchMessage(&msg);
}
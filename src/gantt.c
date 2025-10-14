#include "../include/gantt.h"
#include <stdio.h>

static COLORREF colorForPid(int pid) {
    int r = (pid * 97) % 200 + 40;
    int g = (pid * 57) % 200 + 40;
    int b = (pid * 23) % 200 + 40;
    return RGB(r, g, b);
}

static void DrawTimeline(HDC hdc, RECT client, const char* label, const Slice* s, int n, int makespan, int row) {
    int left = 80;
    int top = 40 + row * 90;
    int height = 50;
    int width = client.right - client.left - left - 20;

    SetBkMode(hdc, TRANSPARENT);
    TextOutA(hdc, 10, top + (height/2 - 8), label, (int)lstrlenA(label));

    if (makespan <= 0) makespan = 1;

    HPEN border = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    HGDIOBJ oldPen = SelectObject(hdc, border);

    // outer lane box
    Rectangle(hdc, left, top, left + width, top + height);

    int lastX = left;
    for (int i = 0; i < n; ++i) {
        int x0 = left + (int)((double)s[i].start / makespan * width);
        int x1 = left + (int)((double)s[i].end   / makespan * width);
        if (x1 < x0) x1 = x0;

        RECT r = { x0, top, x1, top + height };
        HBRUSH br = CreateSolidBrush(colorForPid(s[i].pid));
        FillRect(hdc, &r, br);
        DeleteObject(br);

        // slice separator
        MoveToEx(hdc, x0, top, NULL);
        LineTo(hdc, x0, top + height);

        // pid label centered in slice if wide enough
        if (x1 - x0 > 18) {
            char buf[16]; wsprintfA(buf, "P%d", s[i].pid);
            int tx = x0 + ((x1 - x0) / 2) - 8;
            int ty = top + (height/2 - 8);
            TextOutA(hdc, tx, ty, buf, (int)lstrlenA(buf));
        }
        lastX = x1;
    }
    // right border
    MoveToEx(hdc, left + width, top, NULL);
    LineTo(hdc, left + width, top + height);

    SelectObject(hdc, oldPen);
    DeleteObject(border);

    // axis with ticks at slice boundaries (start times)
    int axisY = top + height + 18;
    MoveToEx(hdc, left, axisY, NULL);
    LineTo(hdc, left + width, axisY);
    int lastT = -1;
    for (int i = 0; i < n; ++i) {
        int t = s[i].start;
        if (t == lastT) continue;
        int x = left + (int)((double)t / makespan * width);
        MoveToEx(hdc, x, axisY - 6, NULL);
        LineTo(hdc, x, axisY + 6);
        char tb[16]; wsprintfA(tb, "%d", t);
        TextOutA(hdc, x - 6, axisY + 8, tb, (int)lstrlenA(tb));
        lastT = t;
    }
    // final makespan label
    char tb[16]; wsprintfA(tb, "%d", makespan);
    TextOutA(hdc, left + width - 10, axisY + 8, tb, (int)lstrlenA(tb));
}

static const Slice* g_fcfs; static int g_n_fcfs;
static const Slice* g_sjf;  static int g_n_sjf;
static const Slice* g_rr;   static int g_n_rr;
static int g_makespan;

static LRESULT CALLBACK GanttProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            HFONT h = CreateFontA(20,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Arial");
            SelectObject(hdc, h);
            TextOutA(hdc, 10, 10, "Gantt Chart", 11);
            DeleteObject(h);
            DrawTimeline(hdc, rc, "FCFS", g_fcfs, g_n_fcfs, g_makespan, 0);
            DrawTimeline(hdc, rc, "SJF",  g_sjf,  g_n_sjf,  g_makespan, 1);
            DrawTimeline(hdc, rc, "RR",   g_rr,   g_n_rr,   g_makespan, 2);
            EndPaint(hwnd, &ps);
        } return 0;
    }
    return DefWindowProc(hwnd, msg, w, l);
}

void ShowGanttWindow(const char* title,
                     const Slice* fcfs, int n_fcfs,
                     const Slice* sjf,  int n_sjf,
                     const Slice* rr,   int n_rr,
                     int makespan) {
    g_fcfs = fcfs; g_n_fcfs = n_fcfs;
    g_sjf  = sjf;  g_n_sjf  = n_sjf;
    g_rr   = rr;   g_n_rr   = n_rr;
    g_makespan = makespan;

    WNDCLASS wc = {0};
    wc.lpfnWndProc = GanttProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "GanttWnd";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(wc.lpszClassName, title ? title : "Gantt",
        WS_OVERLAPPEDWINDOW^WS_THICKFRAME, 160, 160, 900, 360,
        NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
}

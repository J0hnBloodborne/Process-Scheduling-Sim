#include "../include/gantt.h"
#include <stdio.h>

static COLORREF colorForPid(int pid) {
    int r = (pid * 97) % 200 + 40;
    int g = (pid * 57) % 200 + 40;
    int b = (pid * 23) % 200 + 40;
    return RGB(r, g, b);
}

static void DrawTimeline(HDC hdc, RECT client, const char* label, const Slice* s, int n, int makespan, int row, int virtualWidth, int scrollX) {
    int left = 80;
    int top = 50 + row * 120; // increased spacing between rows
    int height = 60;          // slightly taller lanes
    int width = virtualWidth - left - 20; // virtual timeline width

    SetBkMode(hdc, TRANSPARENT);
    TextOutA(hdc, 10, top + (height/2 - 8), label, (int)lstrlenA(label));

    if (makespan <= 0) makespan = 1;

    HPEN border = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    HGDIOBJ oldPen = SelectObject(hdc, border);

    // outer lane box
    // outer lane box (scrolls horizontally)
    Rectangle(hdc, left - scrollX, top, left - scrollX + width, top + height);

    int lastX = left;
    for (int i = 0; i < n; ++i) {
    int x0 = left + (int)((double)s[i].start / makespan * width) - scrollX;
    int x1 = left + (int)((double)s[i].end   / makespan * width) - scrollX;
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
    MoveToEx(hdc, left - scrollX + width, top, NULL);
    LineTo(hdc, left - scrollX + width, top + height);

    SelectObject(hdc, oldPen);
    DeleteObject(border);

    // axis with ticks at slice boundaries (start times)
    int axisY = top + height + 18;
    MoveToEx(hdc, left - scrollX, axisY, NULL);
    LineTo(hdc, left - scrollX + width, axisY);
    int lastT = -1;
    for (int i = 0; i < n; ++i) {
        int t = s[i].start;
        if (t == lastT) continue;
        int x = left + (int)((double)t / makespan * width) - scrollX;
        MoveToEx(hdc, x, axisY - 6, NULL);
        LineTo(hdc, x, axisY + 6);
        char tb[16]; wsprintfA(tb, "%d", t);
        TextOutA(hdc, x - 6, axisY + 8, tb, (int)lstrlenA(tb));
        lastT = t;
    }
    // final makespan label
    char tb[16]; wsprintfA(tb, "%d", makespan);
    TextOutA(hdc, left - scrollX + width - 10, axisY + 8, tb, (int)lstrlenA(tb));
}

static const Slice* g_fcfs; static int g_n_fcfs;
static const Slice* g_sjf;  static int g_n_sjf;
static const Slice* g_rr;   static int g_n_rr;
static int g_makespan;
static int g_canvas_width;
static int g_scrollX = 0;

static void UpdateCanvasScrollInfo(HWND hwnd) {
    RECT rc; GetClientRect(hwnd, &rc);
    SCROLLINFO si = { sizeof(SCROLLINFO) };
    si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS;
    si.nMin = 0;
    // nMax is inclusive; effective max scroll pos will be nMax - (page - 1)
    si.nMax = (g_canvas_width > 0 ? g_canvas_width - 1 : 0);
    si.nPage = (rc.right - rc.left);
    if (si.nPage > (UINT)(si.nMax + 1)) {
        si.nPage = si.nMax + 1; // disable scroll if content smaller than page
        g_scrollX = 0;
    } else if (g_scrollX > (int)(si.nMax - (int)si.nPage + 1)) {
        g_scrollX = max(0, (int)si.nMax - (int)si.nPage + 1);
    }
    si.nPos = g_scrollX;
    SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
}

static LRESULT CALLBACK GanttCanvasProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            HFONT h = CreateFontA(20,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Arial");
            SelectObject(hdc, h);
            TextOutA(hdc, 10, 10, "Gantt Chart", 11);
            DeleteObject(h);
            // Content draws within virtual canvas width and scrolls by g_scrollX
            DrawTimeline(hdc, rc, "FCFS", g_fcfs, g_n_fcfs, g_makespan, 0, g_canvas_width, g_scrollX);
            DrawTimeline(hdc, rc, "SJF",  g_sjf,  g_n_sjf,  g_makespan, 1, g_canvas_width, g_scrollX);
            DrawTimeline(hdc, rc, "RR",   g_rr,   g_n_rr,   g_makespan, 2, g_canvas_width, g_scrollX);
            EndPaint(hwnd, &ps);
        } return 0;
        case WM_SIZE: {
            UpdateCanvasScrollInfo(hwnd);
        } return 0;
        case WM_HSCROLL: {
            SCROLLINFO si = { sizeof(SCROLLINFO) };
            si.fMask = SIF_ALL;
            GetScrollInfo(hwnd, SB_HORZ, &si);
            int pos = si.nPos;
            switch (LOWORD(w)) {
                case SB_LINELEFT:  pos -= 20; break;
                case SB_LINERIGHT: pos += 20; break;
                case SB_PAGELEFT:  pos -= (int)si.nPage; break;
                case SB_PAGERIGHT: pos += (int)si.nPage; break;
                case SB_THUMBTRACK:
                case SB_THUMBPOSITION:
                    pos = si.nTrackPos; break;
            }
            int maxPos = (int)si.nMax - (int)si.nPage + 1;
            if (maxPos < 0) maxPos = 0;
            if (pos < 0) pos = 0; else if (pos > maxPos) pos = maxPos;
            if (pos != (int)si.nPos) {
                si.fMask = SIF_POS;
                si.nPos = pos;
                SetScrollInfo(hwnd, SB_HORZ, &si, TRUE);
                g_scrollX = pos;
                InvalidateRect(hwnd, NULL, TRUE);
            }
        } return 0;
    }
    return DefWindowProc(hwnd, msg, w, l);
}

static LRESULT CALLBACK GanttFrameProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    static HWND hCanvas;
    switch (msg) {
        case WM_CREATE: {
            RECT rc; GetClientRect(hwnd, &rc);
            hCanvas = CreateWindowExA(0, "GanttCanvasWnd", "", WS_CHILD|WS_VISIBLE|WS_HSCROLL,
                0,0, rc.right-rc.left, rc.bottom-rc.top, hwnd, NULL, GetModuleHandle(NULL), NULL);
            // set scroll range based on canvas width
            g_scrollX = 0;
            UpdateCanvasScrollInfo(hCanvas);
        } return 0;
        case WM_SIZE: {
            MoveWindow(hCanvas, 0, 0, LOWORD(l), HIWORD(l), TRUE);
            UpdateCanvasScrollInfo(hCanvas);
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

    // estimate canvas width proportional to makespan
    g_canvas_width = 120 + (g_makespan * 4); // 4px per time unit

    WNDCLASS wc1 = {0};
    wc1.lpfnWndProc = GanttFrameProc;
    wc1.hInstance = GetModuleHandle(NULL);
    wc1.lpszClassName = "GanttFrameWnd";
    wc1.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc1);

    WNDCLASS wc2 = {0};
    wc2.lpfnWndProc = GanttCanvasProc;
    wc2.hInstance = GetModuleHandle(NULL);
    wc2.lpszClassName = "GanttCanvasWnd";
    wc2.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc2);

    // Compute needed height based on row layout in DrawTimeline
    int top0 = 50 + 0 * 120; int lane_h = 60; int axis_extra = 18 + 22; // lane + axis + labels
    int content_h = (top0 + (2 * 120) + lane_h + axis_extra) + 20; // 3 rows (0..2) + bottom pad
    if (content_h < 420) content_h = 420; // minimum comfortable height

    HWND hwnd = CreateWindow("GanttFrameWnd", title ? title : "Gantt",
        WS_OVERLAPPEDWINDOW, 120, 120, 1280, content_h,
        NULL, NULL, GetModuleHandle(NULL), NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
}

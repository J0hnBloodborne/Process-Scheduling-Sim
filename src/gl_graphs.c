#include "../include/gl_graphs.h"
#include <windows.h>
#include <stdio.h>

static double bars[3];
static const char* barlabels[3] = { "FCFS", "SJF", "RR" };
static COLORREF barcolors[3] = { RGB(255,0,127), RGB(127,255,0), RGB(0,127,255) };
static char graphtitle[64];
static char graphinfo[128]; 

#define TITLE_PAD 100

LRESULT CALLBACK GraphWndProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l);
static int s_window_offset = 0; 

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

void ShowBarGraph(const char* title, const char* info, double v0, double v1, double v2) {
    bars[0]=v0; bars[1]=v1; bars[2]=v2;
    snprintf(graphtitle, sizeof(graphtitle), "%s", title);
    snprintf(graphinfo, sizeof(graphinfo), "%s", info);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = GraphWndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = title;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);

    int x = 100 + (s_window_offset * 30);
    int y = 100 + (s_window_offset * 30);
    s_window_offset = (s_window_offset + 1) % 8;

    HWND hwnd = CreateWindow(wc.lpszClassName, title,
        WS_OVERLAPPEDWINDOW^WS_THICKFRAME, x, y, 420, 540,
        NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
}

void ShowGraphWindow(
    int num_processes,
    int quantum,
    double fcfs_wait, double sjf_wait, double rr_wait,
    double fcfs_turn, double sjf_turn, double rr_turn
) {
    char info[512];
    snprintf(info, sizeof(info), "Total processes: %d  q=%d", num_processes, quantum);
    ShowBarGraph("Average Waiting Time", info, fcfs_wait, sjf_wait, rr_wait);

    snprintf(info, sizeof(info), "Total processes: %d  q=%d", num_processes, quantum);
    ShowBarGraph("Average Turnaround Time", info, fcfs_turn, sjf_turn, rr_turn);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0)
        DispatchMessage(&msg);
}

// ===== Combined graphs window =====

typedef struct { const char* title; double v0,v1,v2; } Chart;

static Chart g_charts[4];
static int g_chart_count = 0;
static char g_info_line[128];

static void DrawOneChart(HDC hdc, RECT client, int left, int top, const Chart* c) {
    int margin = 10;
    int barw = 70;
    int gap = 24;
    int title_h = 22;
    int bars_top = top + title_h + 14; // a little more space under the title
    // Compute cell bounds based on 2x2 layout from AllChartsProc
    int content_top = 76;
    int content_bottom = client.bottom - 20;
    int half_h = (content_bottom - content_top) / 2;
    int cell_top = (top < content_top + half_h) ? content_top : (content_top + half_h);
    int cell_bottom = (top < content_top + half_h) ? (content_top + half_h) : content_bottom;
    int desired_bar_area = 180; // target bars area height
    int bars_base = bars_top + desired_bar_area;
    if (bars_base > cell_bottom - 26) bars_base = cell_bottom - 26; // leave room for labels

    SetBkMode(hdc, TRANSPARENT);
    HFONT hTitle = CreateFontA(20,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Arial");
    SelectObject(hdc, hTitle);
    TextOutA(hdc, left + margin, top, c->title, (int)lstrlenA(c->title));
    DeleteObject(hTitle);

    double maxv = c->v0; if (c->v1>maxv) maxv=c->v1; if (c->v2>maxv) maxv=c->v2; if (maxv<=0) maxv=1;
    HFONT hBar = CreateFontA(16,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Arial");
    SelectObject(hdc, hBar);
    int bx = left + margin;
    int bar_max_h = bars_base - bars_top;
    double vals[3] = {c->v0, c->v1, c->v2};
    for (int i=0;i<3;i++) {
        int bh = (int)((vals[i]/maxv) * bar_max_h);
        int bar_top = bars_base - bh;
        HBRUSH brush = CreateSolidBrush(barcolors[i]);
        RECT r = { bx, bar_top, bx+barw, bars_base };
        FillRect(hdc, &r, brush);
        DeleteObject(brush);
        char valbuf[32]; sprintf_s(valbuf, sizeof(valbuf), "%.2f", vals[i]);
        TextOutA(hdc, bx+(barw/2)-20, bar_top-22, valbuf, (int)lstrlenA(valbuf));
        TextOutA(hdc, bx+(barw/2)-20, bars_base+6, barlabels[i], (int)lstrlenA(barlabels[i]));
        bx += barw + gap;
    }
    DeleteObject(hBar);
}

static LRESULT CALLBACK AllChartsProc(HWND hwnd, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc; GetClientRect(hwnd, &rc);
            SetBkMode(hdc, TRANSPARENT);
            HFONT hHead = CreateFontA(22,0,0,0,FW_BOLD,0,0,0,0,0,0,0,0,"Arial");
            SelectObject(hdc, hHead);
            TextOutA(hdc, 20, 14, "Scheduling Metrics", 19);
            DeleteObject(hHead);

            HFONT hInfo = CreateFontA(15,0,0,0,FW_NORMAL,0,0,0,0,0,0,0,0,"Arial");
            SelectObject(hdc, hInfo);
            TextOutA(hdc, 20, 42, g_info_line, (int)lstrlenA(g_info_line));
            DeleteObject(hInfo);

            // 2x2 grid layout with divider lines
            int content_left = 20;
            int content_right = rc.right - 20;
            int content_top = 76;
            int content_bottom = rc.bottom - 20;
            int mid_x = (content_left + content_right) / 2;
            int mid_y = content_top + (content_bottom - content_top) / 2;

            // Draw divider lines (vertical and horizontal)
            HPEN hPen = CreatePen(PS_DOT, 1, RGB(180,180,180));
            HGDIOBJ oldPen = SelectObject(hdc, hPen);
            MoveToEx(hdc, mid_x, content_top, NULL);
            LineTo(hdc, mid_x, content_bottom);
            MoveToEx(hdc, content_left, mid_y, NULL);
            LineTo(hdc, content_right, mid_y);
            SelectObject(hdc, oldPen);
            DeleteObject(hPen);

            // Chart positions (TL, TR, BL, BR)
            int cell_pad_x = 12;
            int cell_pad_y = 10;
            int tl_left = content_left + cell_pad_x;
            int tl_top  = content_top + cell_pad_y;
            int tr_left = mid_x + cell_pad_x;
            int tr_top  = content_top + cell_pad_y;
            int bl_left = content_left + cell_pad_x;
            int bl_top  = mid_y + cell_pad_y;
            int br_left = mid_x + cell_pad_x;
            int br_top  = mid_y + cell_pad_y;

            if (g_chart_count > 0) DrawOneChart(hdc, rc, tl_left, tl_top, &g_charts[0]);
            if (g_chart_count > 1) DrawOneChart(hdc, rc, tr_left, tr_top, &g_charts[1]);
            if (g_chart_count > 2) DrawOneChart(hdc, rc, bl_left, bl_top, &g_charts[2]);
            if (g_chart_count > 3) DrawOneChart(hdc, rc, br_left, br_top, &g_charts[3]);
            EndPaint(hwnd, &ps);
        } return 0;
    }
    return DefWindowProc(hwnd, msg, w, l);
}

void ShowAllGraphsWindow(
    int num_processes,int quantum,
    double fcfs_wait, double sjf_wait, double rr_wait,
    double fcfs_turn, double sjf_turn, double rr_turn,
    double fcfs_resp, double sjf_resp, double rr_resp,
    int ctx_fcfs, int ctx_sjf, int ctx_rr) {

    g_chart_count = 0;
    wsprintfA(g_info_line, "Total processes: %d  q=%d", num_processes, quantum);
    g_charts[g_chart_count++] = (Chart){"Average Response Time", fcfs_resp, sjf_resp, rr_resp};
    g_charts[g_chart_count++] = (Chart){"Context Switches", (double)ctx_fcfs, (double)ctx_sjf, (double)ctx_rr};
    g_charts[g_chart_count++] = (Chart){"Average Waiting Time", fcfs_wait, sjf_wait, rr_wait};
    g_charts[g_chart_count++] = (Chart){"Average Turnaround Time", fcfs_turn, sjf_turn, rr_turn};

    WNDCLASS wc = {0};
    wc.lpfnWndProc = AllChartsProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "AllChartsWnd";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);

    int x = 80 + (s_window_offset * 30);
    int y = 80 + (s_window_offset * 30);
    s_window_offset = (s_window_offset + 1) % 8;
    HWND hwnd = CreateWindow(wc.lpszClassName, "All Metrics",
        WS_OVERLAPPEDWINDOW^WS_THICKFRAME, x, y, 1100, 560,
        NULL, NULL, wc.hInstance, NULL);
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
}
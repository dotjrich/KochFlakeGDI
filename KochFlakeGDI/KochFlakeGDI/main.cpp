//
// KochFlakeGDI - GDI+ based Koch Snowflake.
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <objidl.h>
#include <gdiplus.h>

#include <vector>

#include "Line.hpp"

// -----------------------------------------------------------------------

TCHAR g_szWindowClassName[] = _T("KochFlakeGDI");
TCHAR g_szWindowTitle[] = _T("Koch Flake GDI+");

std::vector<Line> g_lines;

// -----------------------------------------------------------------------

void
InitGeometry();

LRESULT CALLBACK
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

void
OnPaint(HDC hdc);

// -----------------------------------------------------------------------

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    InitGeometry();

    WNDCLASSEX wcex;
    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = g_szWindowClassName;

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, _T("Failed to register window class."), g_szWindowTitle, NULL);
        return 1;
    }

    HWND hWnd = CreateWindow(g_szWindowClassName, g_szWindowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 600, 600, NULL, NULL, hInstance, NULL);
    if (!hWnd) {
        MessageBox(NULL, _T("Failed to create window."), g_szWindowTitle, NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    bool run = true;
    MSG msg;
    while (run) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                run = false;
            }

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
        }
    }

    // XXX: Need to clear out the vector, so the m_pen destructors run before we shutdown GDI+.
    g_lines.clear();

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return (int)msg.wParam;
}

// -----------------------------------------------------------------------

void
InitGeometry()
{
    // Initialize our lines.
    g_lines.push_back(Line(Gdiplus::Point(100, 500), Gdiplus::Point(300, 100), Gdiplus::Color(255, 255, 0, 0)));
    g_lines.push_back(Line(Gdiplus::Point(300, 100), Gdiplus::Point(500, 500), Gdiplus::Color(255, 0, 255, 0)));
    g_lines.push_back(Line(Gdiplus::Point(500, 500), Gdiplus::Point(100, 500), Gdiplus::Color(255, 0, 0, 255)));
}

// -----------------------------------------------------------------------

LRESULT CALLBACK
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int ret = 0;
    HDC hdc;
    PAINTSTRUCT ps;

    switch (msg) {
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        OnPaint(hdc);
        EndPaint(hWnd, &ps);
        ret = 0;
        break;
        
    case WM_DESTROY:
        PostQuitMessage(0);
        ret = 0;
        break;

    default:
        ret = DefWindowProc(hWnd, msg, wParam, lParam);
        break;
    }

    return ret;
}

// -----------------------------------------------------------------------

void
OnPaint(HDC hdc)
{
    std::vector<Line>::iterator i;
    for (i = g_lines.begin(); i != g_lines.end(); ++i) {
        i->draw(hdc);
    }
}

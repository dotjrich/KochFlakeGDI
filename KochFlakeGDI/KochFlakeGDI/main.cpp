//
// KochFlakeGDI - GDI+ based Koch Snowflake.
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <objidl.h>
#include <gdiplus.h>

#include <cmath>
#include <vector>

#include "Line.hpp"

// -----------------------------------------------------------------------
// Constants.
// -----------------------------------------------------------------------

const TCHAR WINDOW_CLASS_NAME[] = _T("KochFlakeGDI");
const TCHAR WINDOW_TITLE[] = _T("Koch Flake GDI+");
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

const double SIXTY_DEG_IN_RAD = 3.14159f / 3.0f;

// -----------------------------------------------------------------------
// Globals.
// -----------------------------------------------------------------------

// The list of lines to manipulate / draw.
std::vector<Line> g_lines;

// -----------------------------------------------------------------------

void
InitGeometry();

void
SplitLine(const Line& line, std::vector<Line>& dest);

void
BuildTriangle(const Line& base, std::vector<Line>& dest);

void
Iterate();

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
    wcex.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hIconSm        = LoadIcon(wcex.hInstance, IDI_APPLICATION);
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = WINDOW_CLASS_NAME;

    if (!RegisterClassEx(&wcex)) {
        MessageBox(NULL, _T("Failed to register window class."), WINDOW_TITLE, NULL);
        return 1;
    }

    HWND hWnd = CreateWindow(WINDOW_CLASS_NAME, WINDOW_TITLE, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL, NULL, hInstance, NULL);
    if (!hWnd) {
        MessageBox(NULL, _T("Failed to create window."), WINDOW_TITLE, NULL);
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
    g_lines.push_back(Line(Gdiplus::PointF(200.0f, 500.0f), Gdiplus::PointF(400.0f, 100.0f), Gdiplus::Color(255, 255, 0, 0)));
    g_lines.push_back(Line(Gdiplus::PointF(400.0f, 100.0f), Gdiplus::PointF(600.0f, 500.0f), Gdiplus::Color(255, 0, 255, 0)));
    g_lines.push_back(Line(Gdiplus::PointF(600.0f, 500.0f), Gdiplus::PointF(200.0f, 500.0f), Gdiplus::Color(255, 0, 0, 255)));
}

// -----------------------------------------------------------------------

void
SplitLine(const Line& line, std::vector<Line>& dest)
{
    Gdiplus::Color color;
    line.getColor(color);

    Gdiplus::PointF p1 = line.p1;
    Gdiplus::PointF p2 = line.p2;

    Gdiplus::PointF delta((p2.X - p1.X) / 3.0f, (p2.Y - p1.Y) / 3.0f);

    Line l1(p1, p1 + delta, color);
    Line l2(l1.p2, l1.p2 + delta, color);
    Line l3(l2.p2, p2, color);

    dest.push_back(l1);
    BuildTriangle(l2, dest);
    dest.push_back(l3);
}

// -----------------------------------------------------------------------

// The name of this function is a bit misleading... we're not building a full triangle. Just the new parts to form the Koch curve.
void
BuildTriangle(const Line& base, std::vector<Line>& dest)
{
    Gdiplus::Color color;
    base.getColor(color);

    float deltaX = base.p2.X - base.p1.X;
    float deltaY = base.p2.Y - base.p1.Y;
    float length = ::sqrtf((deltaX * deltaX) + (deltaY * deltaY));
    
    /*
        Some explaining...
        atan2 will give us the angle between X+ and the specified point.
        In this case, we're passing in the midpoint of our segment.
        We're then rotating it 60 deg. This will lead us to the missing point of our triangle.
    */
    float t = ::atan2f(deltaY / 2.0f, deltaX / 2.0f) - SIXTY_DEG_IN_RAD;

    // With the angle in hand, we can locate the missing top of our triangle.
    Gdiplus::PointF topPoint(base.p1.X + (length * ::cosf(t)),
                             base.p1.Y + (length * ::sinf(t)));

    Line l1(base.p1, topPoint, color);
    Line l2(topPoint, base.p2, color);

    dest.push_back(l1);
    dest.push_back(l2);
}

// -----------------------------------------------------------------------

// Take the curve to the next level.
void
Iterate()
{
        std::vector<Line> newLines;
        for (auto i = g_lines.begin(); i != g_lines.end(); ++i) {
            SplitLine(*i, newLines);
        }

        g_lines = newLines;
}

// -----------------------------------------------------------------------

LRESULT CALLBACK
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    LRESULT ret = 0;
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

    case WM_KEYDOWN:
        switch (wParam) {
        case VK_SPACE:
            Iterate();
            InvalidateRect(hWnd, 0, TRUE);
            break;

        default:
            break;
        }

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

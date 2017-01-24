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

// The list of points that make up the lines.
std::vector<Gdiplus::PointF> g_points;

// Pen used to draw lines.
Gdiplus::Pen* g_linePen;

// -----------------------------------------------------------------------

void
InitGeometry();

void
SplitLine(const Gdiplus::PointF& a, const Gdiplus::PointF& b, std::vector<Gdiplus::PointF>& dest);

void
BuildTriangle(const Gdiplus::PointF& a, const Gdiplus::PointF& b, std::vector<Gdiplus::PointF>& dest);

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

    // Build the pen we'll be drawing the line with. Use a horizontal linear gradient.
    g_linePen = new Gdiplus::Pen(&Gdiplus::LinearGradientBrush(Gdiplus::PointF(100.0f, 10.0f), Gdiplus::PointF(700.0f, 10.0f), Gdiplus::Color(255, 0, 255, 0), Gdiplus::Color(255, 0, 0, 255)),
                                 3.0f);
    // Round caps, so the lines blend together nicely.
    g_linePen->SetStartCap(Gdiplus::LineCapRound);
    g_linePen->SetEndCap(Gdiplus::LineCapRound);

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

    g_linePen;

    Gdiplus::GdiplusShutdown(gdiplusToken);

    return (int)msg.wParam;
}

// -----------------------------------------------------------------------

void
InitGeometry()
{
    g_points.push_back(Gdiplus::PointF(200.0f, 500.0f));
    g_points.push_back(Gdiplus::PointF(400.0f, 100.0f));
    g_points.push_back(Gdiplus::PointF(600.0f, 500.0f));
    g_points.push_back(Gdiplus::PointF(200.0f, 500.0f));
}

// -----------------------------------------------------------------------

void
SplitLine(const Gdiplus::PointF& a, const Gdiplus::PointF& b, std::vector<Gdiplus::PointF>& dest)
{
    Gdiplus::PointF delta((b.X - a.X) / 3.0f, (b.Y - a.Y) / 3.0f);

    Gdiplus::PointF p1 = a + delta;
    Gdiplus::PointF p2 = p1 + delta;

    dest.push_back(a);
    dest.push_back(p1);
    BuildTriangle(p1, p2, dest);
    dest.push_back(p2);
    dest.push_back(b);
}

// -----------------------------------------------------------------------

// The name of this function is a bit misleading... we're not building a full triangle. Just the new parts to form the Koch curve.
void
BuildTriangle(const Gdiplus::PointF& a, const Gdiplus::PointF& b, std::vector<Gdiplus::PointF>& dest)
{
    float deltaX = b.X - a.X;
    float deltaY = b.Y - a.Y;
    float length = ::sqrtf((deltaX * deltaX) + (deltaY * deltaY));
    
    /*
        Some explaining...
        atan2 will give us the angle between X+ and the specified point.
        In this case, we're passing in the midpoint of our segment.
        We're then rotating it 60 deg. This will lead us to the missing point of our triangle.
    */
    float t = ::atan2f(deltaY / 2.0f, deltaX / 2.0f) - SIXTY_DEG_IN_RAD;

    // With the angle in hand, we can locate the missing top of our triangle.
    Gdiplus::PointF topPoint(a.X + (length * ::cosf(t)),
                             a.Y + (length * ::sinf(t)));

    dest.push_back(topPoint);
}

// -----------------------------------------------------------------------

// Take the curve to the next level.
void
Iterate()
{
        std::vector<Gdiplus::PointF> newPoints;
        for (int i = 0; i < g_points.size() - 1; ++i) {
            SplitLine(g_points[i], g_points[i + 1], newPoints);
        }

        g_points = newPoints;
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

        case VK_ESCAPE:
            g_points.clear();
            InitGeometry();
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
    Gdiplus::Graphics graphics(hdc);
    graphics.DrawLines(g_linePen, g_points.data(), g_points.size());
}

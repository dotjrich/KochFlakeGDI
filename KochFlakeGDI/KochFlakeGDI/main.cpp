//
// KochFlakeGDI - GDI+ based Koch Snowflake.
//

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>

// -----------------------------------------------------------------------

TCHAR g_szWindowClassName[] = _T("KochFlakeGDI");
TCHAR g_szWindowTitle[] = _T("Koch Flake GDI+");

// -----------------------------------------------------------------------

LRESULT CALLBACK
WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

// -----------------------------------------------------------------------

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
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

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

// -----------------------------------------------------------------------

LRESULT CALLBACK
WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    int ret = 0;

    switch (msg) {
    case WM_PAINT:
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

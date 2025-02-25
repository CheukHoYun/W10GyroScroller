#include "TrayIcon.h"
#include "MouseLocker.h"
#include <shellapi.h>
#include <cstdio>

namespace TrayIcon
{
    // Window procedure to handle tray icon messages.
    LRESULT CALLBACK TrayWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
            case WM_TRAYICON:
                if (lParam == WM_RBUTTONUP)
                { // Right-click on tray icon.
                    // Create a popup menu.
                    HMENU hMenu = CreatePopupMenu();

                    // Format and add a disabled menu item showing the sensitivity.
                    char sensitivityText[64];
                    sprintf(sensitivityText, "Sensitivity: %.4f", MouseLocker::getSensitivity());
                    AppendMenu(hMenu, MF_STRING | MF_DISABLED, 0, sensitivityText);
                    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

                    // Add the clickable Exit menu item.
                    AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, "Exit");

                    // Get the cursor position and show the menu.
                    POINT pt;
                    GetCursorPos(&pt);
                    SetForegroundWindow(hWnd); // Ensure the menu displays correctly.
                    TrackPopupMenu(hMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
                    DestroyMenu(hMenu);
                }
                break;

            case WM_COMMAND:
                if (LOWORD(wParam) == ID_TRAY_EXIT)
                {
                    // Exit the application.
                    PostQuitMessage(0);
                }
                break;

            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
        }
        return 0;
    }

    // Creates a hidden window to own the tray icon and installs the icon.
    HWND createTrayWindow(HINSTANCE hInstance)
    {
        const char *CLASS_NAME = "TrayIconWindowClass";

        // Register a window class for our hidden tray icon window.
        WNDCLASS wc = {0};
        wc.lpfnWndProc = TrayWndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;
        RegisterClass(&wc);

        // Create a hidden window.
        HWND hWnd = CreateWindowEx(0, CLASS_NAME, "Tray Icon Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                                   CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

        // Set up the tray icon.
        NOTIFYICONDATA nid = {0};
        nid.cbSize = sizeof(nid);
        nid.hWnd = hWnd;
        nid.uID = 1; // Tray icon identifier.
        nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
        nid.uCallbackMessage = WM_TRAYICON;
        nid.hIcon = LoadIcon(NULL, IDI_APPLICATION); // Use default application icon.
        strcpy(nid.szTip, "G10 Gyro Scroller");
        Shell_NotifyIcon(NIM_ADD, &nid);

        return hWnd;
    }

    // Cleanup function to remove the tray icon and destroy the window.
    void cleanupTrayIcon(HWND hWnd)
    {
        NOTIFYICONDATA nid = {0};
        nid.cbSize = sizeof(nid);
        nid.hWnd = hWnd;
        nid.uID = 1;
        Shell_NotifyIcon(NIM_DELETE, &nid);
        DestroyWindow(hWnd);
    }
}

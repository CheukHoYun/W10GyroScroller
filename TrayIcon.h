#ifndef TRAY_ICON_H
#define TRAY_ICON_H

#include <windows.h>

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001

namespace TrayIcon{
    HWND createTrayWindow(HINSTANCE hInstance);
    void cleanupTrayIcon(HWND hWnd);
}

#endif // TRAY_ICON_H

#include <iostream>
#include <sstream>
#include "TrayIcon.h"
#include "KeyboardHook.h"
#include "MouseLocker.h"
#include <windows.h>

int main() {
    // Read sensitivity from config file or use default.
    MouseLocker::loadSensitivity("config.cfg", 0.005);

    // Register callbacks for the keyboard and mouse hooks.
    KeyboardHook::registerBothHeldStartCallback(MouseLocker::lockMouse);
    KeyboardHook::registerBothHeldEndCallback(MouseLocker::unlockMouse);

    // Install the keyboard hook.
    if (!KeyboardHook::installHook()) {
        std::cerr << "Error: Failed to install keyboard hook. Exiting." << std::endl;
        return -1;
    }

    // Create the tray icon window.
    HINSTANCE hInstance = GetModuleHandle(nullptr);
    HWND trayWindow = TrayIcon::createTrayWindow(hInstance);
    if (!trayWindow) {
        std::cerr << "Error: Failed to create tray icon window. Exiting." << std::endl;
        KeyboardHook::uninstallHook();
        MouseLocker::unlockMouse();
        return -1;
    }

    // Standard message loop.
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cleanup
    KeyboardHook::uninstallHook();
    MouseLocker::unlockMouse();
    TrayIcon::cleanupTrayIcon(trayWindow);

    return 0;
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    return main();
}
#endif
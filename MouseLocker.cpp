#include <windows.h>
#include <cmath>
#include <fstream>
#include <sstream>
#include "SmoothScroller.h"
#include "Logger.h"

namespace MouseLocker {

    namespace {
        bool isLocked = false;
        bool anchorInitialized = false;
        LONG anchorY = 0;
        int accumulatedDelta = 0;
        HHOOK hMouseHook = nullptr;
        double sensitivity = 0.0005;
        int lastScrollSpeed = 0;
    }

    double getSensitivity() {
        return sensitivity;
    }

    // Helper function to load sensitivity from a config file.
    void loadSensitivity(const char* filename, double defaultSensitivity) {
        std::ifstream configFile(filename);
        if (!configFile) {
            // Config file not found; return default value.
            sensitivity = defaultSensitivity;
            return;
        }
        std::string line;
        while (std::getline(configFile, line)) {
            // A simple parser: expect a line like "sensitivity=0.0010"
            std::istringstream iss(line);
            std::string key;
            if (std::getline(iss, key, '=')) {
                if (key == "sensitivity") {
                    std::string value;
                    if (std::getline(iss, value)) {
                        sensitivity = std::stod(value);
                        return;
                    }
                }
            }
        }
        sensitivity = defaultSensitivity;
   }



    // Low-level mouse procedure that intercepts mouse movement while locked.
    LRESULT CALLBACK LowLevelMouseProc(int nCode, WPARAM wParam, LPARAM lParam)
    {
        if (nCode == HC_ACTION && isLocked && wParam == WM_MOUSEMOVE)
        {
            auto* pMouseStruct = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);
            if (pMouseStruct)
            {
                // Use the first received mouse Y position as the anchor.
                if (!anchorInitialized)
                {
                    anchorY = pMouseStruct->pt.y;
                    anchorInitialized = true;
                    Logger::log("Anchor Y initialized from mouse hook: " + std::to_string(anchorY));
                }

                // Log the raw Y coordinate from the mouse event.
                Logger::log("Raw mouse Y: " + std::to_string(pMouseStruct->pt.y));

                // Calculate the frame delta based on the anchored Y position.
                int frameDelta = anchorY - pMouseStruct->pt.y;
                Logger::log("Computed frameDelta: " + std::to_string(frameDelta) +
                            " (anchorY: " + std::to_string(anchorY) + ")");

                accumulatedDelta += frameDelta;
                Logger::log("New accumulatedDelta: " + std::to_string(accumulatedDelta));

                // Compute a quadratic scroll speed for smoother response.
                int absDelta = std::abs(accumulatedDelta);
                int quadraticSpeed = (accumulatedDelta >= 0 ? 1 : -1) * (absDelta * absDelta);
                int scrollSpeed = static_cast<int>(quadraticSpeed * sensitivity);

                // Log the computed quadraticSpeed and scrollSpeed.
                Logger::log("quadraticSpeed: " + std::to_string(quadraticSpeed) +
                            ", scrollSpeed: " + std::to_string(scrollSpeed));

                // Update only if the scroll speed has changed.
                if (scrollSpeed != lastScrollSpeed)
                {
                    lastScrollSpeed = scrollSpeed;
                    SmoothScroller::instance().set_scroll_speed(scrollSpeed);
                    Logger::log("Updated Scroll Speed -> Frame Delta: " + std::to_string(frameDelta) +
                                ", Accumulated Delta: " + std::to_string(accumulatedDelta) +
                                ", Scroll Speed: " + std::to_string(scrollSpeed));
                }
            }
            // Block the mouse event so the cursor remains locked.
            return 1;
        }
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // Locks the mouse by installing a low-level mouse hook.
    void lockMouse()
    {
        Logger::log("lockMouse() called.");
        if (!isLocked)
        {
            isLocked = true;
            // Reset the anchor and accumulated values.
            anchorInitialized = false;
            accumulatedDelta = 0;
            lastScrollSpeed = 0;
            hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, nullptr, 0);
            if (hMouseHook)
            {
                Logger::log("Low-level mouse hook installed successfully.");
            }
            else
            {
                Logger::log("Failed to install low-level mouse hook.");
            }
        }
        else {
            Logger::log("lockMouse() called, but mouse is already locked.");
        }
    }

    // Unlocks the mouse by removing the hook and resetting scroll speed.
    void unlockMouse()
    {
        Logger::log("unlockMouse() called.");
        if (isLocked)
        {
            isLocked = false;
            if (hMouseHook)
            {
                UnhookWindowsHookEx(hMouseHook);
                hMouseHook = nullptr;
                Logger::log("Mouse hook uninstalled.");
            }
            else {
                Logger::log("Mouse hook was null during unlock.");
            }
            lastScrollSpeed = 0;
            SmoothScroller::instance().set_scroll_speed(0);
            Logger::log("Reset lastScrollSpeed and scroll speed in SmoothScroller.");
        }
        else {
            Logger::log("unlockMouse() called, but mouse was not locked.");
        }
    }

} // namespace MouseLocker

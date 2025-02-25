#include "Logger.h"
#include <map>
#include <functional>
#include <utility>
#include <windows.h>

namespace KeyboardHook {

    namespace {
        HHOOK hHook = nullptr;

        enum class State {
            Idle,
            WaitingForEDown,
            BothHeld,
            WaitingForEUp,
        };

        State currentState = State::Idle;
        bool processingAllowed = true; // Reentrancy guard

        // Callback functions for state transitions
        std::function<void()> onBothHeldStart;
        std::function<void()> onBothHeldEnd;

        // Utility maps for logging key events
        const std::map<WPARAM, std::string> wParamMap = {
                { WM_KEYDOWN, "down" },
                { WM_KEYUP, "up" }
        };

        const std::map<DWORD, std::string> vkCodeMap = {
                { VK_LWIN, "Win" },
                { 'E', "E" }
        };

        void simulateKeyUpEvent(BYTE vkCode)
        {
            keybd_event(vkCode, 0, KEYEVENTF_KEYUP, 0);
        }
    }

    LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
        auto* kbStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        std::string keyName = vkCodeMap.count(kbStruct->vkCode) ? vkCodeMap.at(kbStruct->vkCode) : "Unknown";
        std::string action = wParamMap.count(wParam) ? wParamMap.at(wParam) : "Unknown";
        Logger::log(keyName + " " + action + (processingAllowed ? " processed" : " not processed"));

        if (nCode >= 0 && processingAllowed) {
            processingAllowed = false;

            switch (currentState) {
                case State::Idle:
                    if (wParam == WM_KEYDOWN && kbStruct->vkCode == VK_LWIN) {
                        Logger::log("Idle -> Waiting for E");
                        currentState = State::WaitingForEDown;
                    }
                    processingAllowed = true;
                    break;

                case State::WaitingForEDown:
                    if (((wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) &&
                         kbStruct->vkCode != 'E' && kbStruct->vkCode != VK_LWIN) ||
                        (wParam == WM_KEYUP && kbStruct->vkCode == VK_LWIN)) {
                        Logger::log("Waiting for E -> Idle");
                        currentState = State::Idle;
                        processingAllowed = true;
                    } else if (wParam == WM_KEYDOWN && kbStruct->vkCode == 'E') {
                        Logger::log("Waiting for E -> Both Held");
                        currentState = State::BothHeld;
                        if (onBothHeldStart) {
                            onBothHeldStart();
                        }
                        processingAllowed = true;
                        Logger::log("Data Intercepted\n");
                        return 1;
                    }
                    break;

                case State::BothHeld:
                    if (wParam == WM_KEYUP && kbStruct->vkCode == VK_LWIN) {
                        Logger::log("Simulating E Up");
                        simulateKeyUpEvent('E');

                        Logger::log("Simulating Win Up");
                        simulateKeyUpEvent(VK_LWIN);

                        processingAllowed = true;
                        currentState = State::WaitingForEUp;
                        if (onBothHeldEnd) {
                            onBothHeldEnd();
                        }
                        Logger::log("Intercepted manual Win press");
                        Logger::log("Both Held -> Waiting for E Up\n");
                        return 1;
                    } else {
                        processingAllowed = true;
                        Logger::log("Keyboard input blocked while both E and Win held");
                        Logger::log("Data Intercepted\n");
                        return 1;
                    }

                case State::WaitingForEUp:
                    if (wParam == WM_KEYUP && kbStruct->vkCode == 'E') {
                        Logger::log("Waiting for E Up -> Idle");
                        currentState = State::Idle;
                    }
                    Logger::log("Data Intercepted\n");
                    processingAllowed = true;
                    return 1;
            }
        }

        Logger::log("Data passthrough\n\n");
        return CallNextHookEx(hHook, nCode, wParam, lParam);
    }

    bool installHook() {
        hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, nullptr, 0);
        if (hHook == nullptr) {
            Logger::log("Failed to install hook!");
            return false;
        }
        return true;
    }

    void uninstallHook() {
        if (hHook != nullptr) {
            UnhookWindowsHookEx(hHook);
        }
    }

    void registerBothHeldStartCallback(std::function<void()> callback) {
        onBothHeldStart = std::move(callback);
    }

    void registerBothHeldEndCallback(std::function<void()> callback) {
        onBothHeldEnd = std::move(callback);
    }

} // namespace KeyboardHook

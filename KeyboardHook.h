#ifndef KEYBOARDHOOK_H
#define KEYBOARDHOOK_H

#include <functional>
#include <windows.h>

namespace KeyboardHook {
    bool installHook();
    void uninstallHook();
    void registerBothHeldStartCallback(std::function<void()> callback);
    void registerBothHeldEndCallback(std::function<void()> callback);
} // namespace KeyboardHook

#endif // KEYBOARDHOOK_H

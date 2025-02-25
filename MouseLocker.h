#ifndef MOUSELOCKER_H
#define MOUSELOCKER_H

namespace MouseLocker {
    void lockMouse();
    void unlockMouse();
    double getSensitivity();
    void loadSensitivity(const char* filename, double defaultSensitivity);
} // namespace MouseLocker

#endif // MOUSELOCKER_H

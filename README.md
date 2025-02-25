# G10 Gyro Scroller

G10 Gyro Scroller is a lightweight Windows utility designed to enable smooth scrolling using the [PepperJobs W10 Air Remote](https://www.pepper-jobs.com/products/w10-gyro-smart-remote). With this tool, you can scroll by moving the mouse vertically while holding down the File Explorer button.

https://github.com/user-attachments/assets/2c36bbe8-e384-473d-8760-9695709471ae





## Code Overview

- **main.cpp**  
  Reads the configuration file, installs the keyboard hook, creates the tray icon, and manages the main message loop.

- **KeyboardHook.cpp**  
  Implements a low-level keyboard hook to monitor key events. It detects when the Win and E keys are pressed together, intercepting further input and triggering callbacks to start or stop scrolling. You may edit this file to rebind the scrolling feature to a different key combination if you like to. 

- **MouseLocker.cpp**  
  Manages mouse locking during scrolling. It captures vertical mouse movement, calculates scrolling deltas, and feeds them into the smooth scrolling mechanism.

- **SmoothScroller.cpp**  
  Oversees smooth scrolling by gradually adjusting the scroll speed based on accumulated mouse movement. This functionality runs on a separate thread to ensure fluid performance.

- **TrayIcon.cpp**  
  Creates and manages the system tray icon. It also provides a right-click menu to adjust sensitivity settings and exit the application.

## How to Use

1. **Preparation**  
   - Compile the project to produce the executable (.exe) file.
   - Ensure the configuration file (`config.cfg`) is located in the same folder as the executable. The file should contain a single line in the format:  
     `sensitivity=<value>`
   - Alternatively, if you prefer not to compile the program yourself, you can download the precompiled .zip file from the [release page](https://github.com/CheukHoYun/W10GyroScroller/releases/tag/Release), which includes both the executable and the configuration file.

2. **Execution**  
   - Run the executable to start the application. It will automatically install the necessary hooks and display a tray icon for easy access.
   - You may now move your mouse up/down to scroll through pages while holding the "File Explorer" (Win+E) button.
   - You may change the sensitivity value in `config.cfg` to allow faster / slower scrolling. A restart of the program is required for it to take effect. 

3. **Auto-Start Setup**  
   - For convenience, you can add the executable and the configuration file to your system’s startup folder so that the application launches automatically when Windows starts.
   - **Startup folder location (for the current user):**  
     `%APPDATA%\Microsoft\Windows\Start Menu\Programs\Startup`

## Disclaimer

This software was developed solely for personal use with the Pepper Jobs W10 Air Remote. I am not affiliated with Pepper Jobs and was not involved in the development of the remote. The software is provided free of charge and is intended for personal use only. You may use, modify, and distribute the program for non-commercial purposes.

---

Happy scrolling!

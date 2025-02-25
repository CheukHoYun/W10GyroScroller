cmake_minimum_required(VERSION 3.27)
project(W10GyroScroller)

#add_definitions(-DDEBUG_MODE)

#add_definitions(-DUNICODE -D_UNICODE)


set(CMAKE_CXX_STANDARD 17)

add_executable(W10GyroScroller WIN32
        main.cpp
        SmoothScroller.cpp
        KeyboardHook.cpp
        KeyboardHook.h
        MouseLocker.cpp
        MouseLocker.h
        Logger.cpp
        Logger.h
        TrayIcon.h
        TrayIcon.cpp
)



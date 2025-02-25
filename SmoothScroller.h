#ifndef SMOOTHSCROLLER_H
#define SMOOTHSCROLLER_H

#include <atomic>
#include <thread>
#include <chrono>
#include <windows.h>

class SmoothScroller
{
public:
    static SmoothScroller& instance();

    void set_scroll_speed(int scroll_speed);

private:
    SmoothScroller();

    void start_thread();
    void scroll_thread();
    void update_scroll_state();
    static void scroll_once(int scroll_increment);

    std::atomic<int> _scroll_speed;
    std::atomic<double> _accumulated_scroll_amount;
    std::atomic<bool> _thread_initialized;
    std::thread _scrolling_thread;
    std::chrono::time_point<std::chrono::high_resolution_clock> _stopwatch_start;
};

#endif // SMOOTHSCROLLER_H

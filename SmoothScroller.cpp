#include "SmoothScroller.h"
#include <iostream>

SmoothScroller& SmoothScroller::instance()
{
    static SmoothScroller instance;
    return instance;
}

void SmoothScroller::set_scroll_speed(int scroll_speed)
{
    if (_scroll_speed == 0 && scroll_speed != 0)
    {
        start_thread();
    }
    _scroll_speed.store(scroll_speed);
}

SmoothScroller::SmoothScroller() : _scroll_speed(0), _accumulated_scroll_amount(0.0), _thread_initialized(false) {}

void SmoothScroller::start_thread()
{
    if (_thread_initialized) return;
    _thread_initialized = true;
    _scrolling_thread = std::thread(&SmoothScroller::scroll_thread, this);
    _scrolling_thread.detach();
}

void SmoothScroller::scroll_thread()
{
    _stopwatch_start = std::chrono::high_resolution_clock::now();
    while (_thread_initialized)
    {
        if (_scroll_speed.load() != 0)
        {
            update_scroll_state();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void SmoothScroller::update_scroll_state()
{
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> delta_time = now - _stopwatch_start;
    _stopwatch_start = now;

    int scroll_speed = _scroll_speed.load();
    if (scroll_speed != 0)
    {
        double added_amount = scroll_speed * delta_time.count();
        double old_accumulated_scroll_amount = _accumulated_scroll_amount.load();
        double new_accumulated_scroll_amount;
        do
        {
            new_accumulated_scroll_amount = old_accumulated_scroll_amount + added_amount;
        } while (!_accumulated_scroll_amount.compare_exchange_weak(old_accumulated_scroll_amount, new_accumulated_scroll_amount));

        int scroll_amount = static_cast<int>(new_accumulated_scroll_amount);
        if (scroll_amount != 0)
        {
            scroll_once(scroll_amount);

            double old_scroll_amount = new_accumulated_scroll_amount;
            do
            {
                new_accumulated_scroll_amount = old_scroll_amount - scroll_amount;
            } while (!_accumulated_scroll_amount.compare_exchange_weak(old_scroll_amount, new_accumulated_scroll_amount));
        }
    }
}

void SmoothScroller::scroll_once(int scroll_increment)
{
    INPUT input;
    input.type = INPUT_MOUSE;
    input.mi.dx = 0;
    input.mi.dy = 0;
    input.mi.mouseData = scroll_increment;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.time = 0;
    input.mi.dwExtraInfo = 0;

    if (SendInput(1, &input, sizeof(INPUT)) == 0)
    {
        std::cerr << "Error sending input." << std::endl;
    }
}

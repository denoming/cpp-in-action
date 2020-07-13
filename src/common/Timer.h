#pragma once

#include <chrono>

class Timer {
public:
    Timer()
        : _point{std::chrono::steady_clock::now()}
    {
    }

    uint64_t
    diff(bool isReset = false)
    {
        auto now{std::chrono::steady_clock::now()};
        std::chrono::duration<double, std::milli> diff{now - _point};
        if (isReset) {
            reset();
        }
        return diff.count();
    }

    void
    reset()
    {
        _point = std::chrono::steady_clock::now();
    }

private:
    std::chrono::steady_clock::time_point _point;
};

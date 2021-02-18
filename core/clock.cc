#include "core/clock.h"

#include <mutex>

namespace avr
{
    void Clock::Pulse()
    {
        _cv.notify_one();
    }

    void Clock::ConsumeCycle()
    {
        std::unique_lock<std::mutex> lock(_m);
        _cv.wait(lock);
    }
}

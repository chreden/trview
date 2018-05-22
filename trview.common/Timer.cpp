#include "Timer.h"
#include <Windows.h>

namespace trview
{
    Timer::Timer()
        : _elapsed(0), _total(0)
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        _frequency = frequency.QuadPart;

        LARGE_INTEGER counter;
        QueryPerformanceCounter(&counter);
        _previous = counter.QuadPart;
    }

    void Timer::update()
    {
        LARGE_INTEGER tick;
        QueryPerformanceCounter(&tick);
        int64_t diff = tick.QuadPart - _previous;
        _elapsed = static_cast< float >(diff) / _frequency;
        _previous = tick.QuadPart;
        _total += _elapsed;
    }

    float Timer::elapsed() const
    {
        return _elapsed;
    }

    float Timer::total() const
    {
        return _total;
    }

    void Timer::reset()
    {
        _elapsed = 0.0f;
        _total = 0.0f;
    }
}

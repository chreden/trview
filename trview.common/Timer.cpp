#include "Timer.h"

namespace trview
{
    Timer::Timer(const std::function<float()>& time_source)
        : _time_source(time_source), _elapsed(0), _total(0), _previous(time_source())
    {
    }

    void Timer::update()
    {
        float value = _time_source();
        _elapsed = value - _previous;
        _total += _elapsed;
        _previous = value;
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
        _previous = _time_source();
    }

    std::function<float()> default_time_source()
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);

        LARGE_INTEGER start;
        QueryPerformanceCounter(&start);

        return [frequency, start]()
        {
            LARGE_INTEGER tick;
            QueryPerformanceCounter(&tick);
            return static_cast<float>(tick.QuadPart - start.QuadPart) / static_cast<float>(frequency.QuadPart);
        };
    }
}

#pragma once

#include <cstdint>

namespace trview
{
    class Timer
    {
    public:
        Timer();
        void  update();
        float elapsed() const;
        float total() const;
        void  reset();
    private:
        int64_t _frequency;
        int64_t _previous;
        float   _elapsed;
        float   _total;
    };
}

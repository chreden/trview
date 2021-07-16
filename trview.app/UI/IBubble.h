#pragma once

#include <trview.common/Point.h>

namespace trview
{
    struct IBubble
    {
        virtual ~IBubble() = 0;
        virtual void show(const Point& position) = 0;
    };
}

#pragma once

namespace trview
{
    namespace ui
    {
        struct Point
        {
            Point(float x, float y);

            Point operator -(const Point& other);

            float x, y;
        };
    }
}
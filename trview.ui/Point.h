#pragma once

namespace trview
{
    namespace ui
    {
        struct Point
        {
            Point();

            Point(float x, float y);

            Point operator -(const Point& other);
            Point operator +(const Point& other);

            bool is_between(const Point& first, const Point& second);

            float x, y;
        };
    }
}
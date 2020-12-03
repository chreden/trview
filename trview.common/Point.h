#pragma once

namespace trview
{
    struct Point
    {
        Point();

        Point(float x, float y);

        Point operator -(const Point& other) const;
        Point operator +(const Point& other) const;
        Point& operator +=(const Point& other);
        bool operator==(const Point& other) const;

        bool is_between(const Point& first, const Point& second) const;

        float x, y;
    };
}
#include "Point.h"

namespace trview
{
    Point::Point()
        : x(0), y(0)
    {
    }

    Point::Point(float x, float y)
        : x(x), y(y)
    {
    }

    Point Point::operator -(const Point& other) const
    {
        return Point(x - other.x, y - other.y);
    }

    Point Point::operator +(const Point& other) const
    {
        return Point(x + other.x, y + other.y);
    }

    bool Point::is_between(const Point& first, const Point& second) const
    {
        return x >= first.x && y >= first.y 
            && x <= second.x && y <= second.y; 
    }
}

#include "Point.h"

namespace trview
{
    namespace ui
    {
        Point::Point()
            : x(0), y(0)
        {
        }

        Point::Point(float x, float y)
            : x(x), y(y)
        {
        }

        Point Point::operator -(const Point& other)
        {
            return Point(x - other.x, y - other.y);
        }
    }
}

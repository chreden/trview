export module trview.common.point;

namespace trview
{
    export struct Point
    {
        Point() : x(0), y(0)
        {
        }

        Point(float x, float y) : x(x), y(y)
        {
        }

        Point operator -(const Point& other) const 
        {
            return Point(x - other.x, y - other.y);
        }

        Point operator +(const Point& other) const
        {
            return Point(x + other.x, y + other.y);
        }

        Point& operator +=(const Point& other)
        {
            x += other.x;
            y += other.y;
            return *this;
        }

        bool operator==(const Point& other) const
        {
            return x == other.x && y == other.y;
        }

        bool is_between(const Point& first, const Point& second) const
        {
            return x >= first.x && y >= first.y
                && x <= second.x && y <= second.y;
        }

        float x, y;
    };
}
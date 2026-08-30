export module trview.common:Point;

import nlohmann.json;

namespace trview
{
    export struct Point
    {
        Point();

        Point(float x, float y);

        Point operator -(const Point& other) const;
        Point operator +(const Point& other) const;
        Point& operator +=(const Point& other);
        bool operator==(const Point& other) const;
        bool operator!=(const Point& other) const;

        bool is_between(const Point& first, const Point& second) const;

        float x, y;
    };

    export void from_json(const nlohmann::json& json, Point& point);
}
#include "JsonSerializers.h"
#include <trview.common/Json.h>

namespace trview
{
    void from_json(const nlohmann::json& json, Size& size)
    {
        size = Size(read_attribute<float>(json, "width"), read_attribute<float>(json, "height"));
    }

    void from_json(const nlohmann::json& json, Point& point)
    {
        point = Point(read_attribute<float>(json, "x"), read_attribute<float>(json, "y"));
    }

    void from_json(const nlohmann::json& json, Colour& colour)
    {
        if (json.is_string())
        {
            const auto str = json.get<std::string>();
            if (str == "transparent")
            {
                colour = Colour::Transparent;
            }
            else
            {
                colour = Colour::White;
            }
        }
        else
        {
            colour = Colour(read_attribute<float>(json, "a", 1.0f),
                read_attribute<float>(json, "r", 0.0f),
                read_attribute<float>(json, "g", 0.0f),
                read_attribute<float>(json, "b", 0.0f));
        }
    }
}

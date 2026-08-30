export module trview.common:JsonSerializers;

import nlohmann.json;

namespace trview
{
    struct Point;
    struct Size;
    struct Colour;

    void from_json(const nlohmann::json& json, Point& point);
    void from_json(const nlohmann::json& json, Size& size);
    void from_json(const nlohmann::json& json, Colour& colour);
    void to_json(nlohmann::json& json, const Colour& colour);
}

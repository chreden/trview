export module trview.common:JsonSerializers;

import nlohmann.json;
import :Point;
import :Size;
import :Colour;

namespace trview
{
    export void from_json(const nlohmann::json& json, Point& point);
    export void from_json(const nlohmann::json& json, Size& size);
    export void from_json(const nlohmann::json& json, Colour& colour);
    export void to_json(nlohmann::json& json, const Colour& colour);
}

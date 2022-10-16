#pragma once

#include <external/nlohmann/json.hpp>

import trview.common;

namespace trview
{
    void from_json(const nlohmann::json& json, Point& point);
    void from_json(const nlohmann::json& json, Size& size);
    void from_json(const nlohmann::json& json, Colour& colour);
    void to_json(nlohmann::json& json, const Colour& colour);
}

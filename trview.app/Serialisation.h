#pragma once

#include "Routing/LevelSignature.h"
#include "Routing/Waypoint.h"
#include <external/nlohmann/json.hpp>

namespace trview
{
    void to_json(nlohmann::json& json, const LevelSignature& signature);
    void to_json(nlohmann::json& json, const Waypoint& waypoint);
}


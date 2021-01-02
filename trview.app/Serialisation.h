#pragma once

#include <external/nlohmann/json.hpp>

namespace trview
{
    struct LevelSignature;
    class Route;
    class Waypoint;

    void to_json(nlohmann::json& json, const LevelSignature& signature);
    void to_json(nlohmann::json& json, const Waypoint& waypoint);
    void to_json(nlohmann::json& json, const Route& route);
}


#pragma once

#include "../../Routing/IWaypoint.h"
#include "../../Settings/UserSettings.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_waypoint(lua_State* L, const std::shared_ptr<IWaypoint>& waypoint);
        void waypoint_register(lua_State* L, IWaypoint::Source source);
        std::shared_ptr<IWaypoint> to_waypoint(lua_State* L, int index);
        void waypoint_set_settings(const UserSettings& settings);
    }
}


#pragma once

#include "../../Routing/IWaypoint.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        void waypoint_register(lua_State* L, IWaypoint::Source source);
        std::shared_ptr<IWaypoint> to_waypoint(lua_State* L, int index);
    }
}


module;

#include <external/lua/src/lua.h>

export module trview.app:LuaWaypoint;

import std;

import :IWaypoint;
import :UserSettings;

namespace trview
{
    namespace lua
    {
        export int create_waypoint(lua_State* L, const std::shared_ptr<IWaypoint>& waypoint);
        export void waypoint_register(lua_State* L, IWaypoint::Source source);
        export std::shared_ptr<IWaypoint> to_waypoint(lua_State* L, int index);
        export void waypoint_set_settings(const UserSettings& settings);
    }
}


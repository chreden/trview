module;

#include <external/lua/src/lua.h>

export module trview.app:LuaWaypoint;

import std;

import :IWaypoint;
import :UserSettings;

namespace trview
{
    export namespace lua
    {
        int create_waypoint(lua_State* L, const std::shared_ptr<IWaypoint>& waypoint);
        void waypoint_register(lua_State* L, IWaypoint::Source source);
        void waypoint_unregister();
        std::shared_ptr<IWaypoint> to_waypoint(lua_State* L, int index);
        void waypoint_set_settings(const UserSettings& settings);
    }
}


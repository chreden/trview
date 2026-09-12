module;

#include <external/lua/src/lua.h>

export module trview.app:LuaRoute;

import std;

import trview.common;

import :IRoute;
import :IRandomizerRoute;

namespace trview
{
    export namespace lua
    {
        int create_route(lua_State* L, const std::shared_ptr<IRoute>& route);
        std::shared_ptr<IRoute> to_route(lua_State* L, int index);
        void route_register(lua_State* L, const IRoute::Source& source, const IRandomizerRoute::Source& randomizer_source, const std::shared_ptr<IDialogs>& dialogs, const std::shared_ptr<IFiles>& files);
        void route_unregister();
        void route_set_settings(const UserSettings& new_settings);
    }
}

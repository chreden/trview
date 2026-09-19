module;

#include <external/lua/src/lua.h>

export module trview.app:LuaLevel;

import std;
import :ILevel;

namespace trview
{
    export namespace lua
    {
        void level_register(lua_State* L);
        int to_lua(lua_State* L, const std::weak_ptr<ILevel>& level);
        std::shared_ptr<ILevel> to_level(lua_State* L, int index);
    }

    export using lua::to_lua;
}
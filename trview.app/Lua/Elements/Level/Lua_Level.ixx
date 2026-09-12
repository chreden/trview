module;

#include <external/lua/src/lua.h>

export module trview.app:LuaLevel;

import std;
import :ILevel;

namespace trview
{
    namespace lua
    {
        export void level_register(lua_State* L);
        export int create_level(lua_State* L, const std::shared_ptr<ILevel>& level);
        export std::shared_ptr<ILevel> to_level(lua_State* L, int index);
    }
}
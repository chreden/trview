module;

#include <external/lua/src/lua.h>

export module trview.app:LuaColour;

import trview.common;

namespace trview
{
    namespace lua
    {
        export int create_colour(lua_State* L, const Colour& value);
        export bool is_colour(lua_State* L, int index);
        export Colour to_colour(lua_State* L, int index);
        export void colour_register(lua_State* L);
    }
}

module;

#include <external/lua/src/lua.h>

export module trview.app:LuaTriangle;

import std;
import :Triangle;

namespace trview
{
    namespace lua
    {
        export int create_triangle(lua_State* L, const Triangle& triangle);
        export void triangle_register(lua_State* L);
    }
}

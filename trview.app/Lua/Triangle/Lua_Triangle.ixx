module;

#include <external/lua/src/lua.h>

export module trview.app:LuaTriangle;

import std;
import :Triangle;

namespace trview
{
    namespace lua
    {
        int create_triangle(lua_State* L, const Triangle& triangle);
        void triangle_register(lua_State* L);
    }
}

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
        export int to_lua(lua_State* L, Triangle::AnimationMode mode);
        export int to_lua(lua_State* L, Triangle::CollisionMode mode);
        export int to_lua(lua_State* L, Triangle::TextureMode mode);
        export int to_lua(lua_State* L, Triangle::TransparencyMode mode);
        export int to_lua(lua_State* L, Triangle::SideMode mode);
    }

    export using lua::to_lua;
}

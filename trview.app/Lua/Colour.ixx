module;

#include <external/lua/src/lua.h>

export module trview.app:LuaColour;

import trview.common;

namespace trview
{
    namespace lua
    {
        export bool is_colour(lua_State* L, int index);
        export Colour to_colour(lua_State* L, int index);
        export void colour_register(lua_State* L);
        export int to_lua(lua_State* L, const Colour& value);
        export int to_lua(lua_State* L, const DirectX::SimpleMath::Color& value);
    }

    export using lua::to_lua;
}

namespace DirectX
{
    namespace SimpleMath
    {
        export using trview::lua::to_lua;
    }
}

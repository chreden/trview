#pragma once

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_colour(lua_State* L, const Colour& value);
        bool is_colour(lua_State* L, int index);
        Colour to_colour(lua_State* L, int index);
        void colour_register(lua_State* L);
    }
}

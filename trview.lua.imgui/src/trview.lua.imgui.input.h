#pragma once

struct lua_State;

namespace trview
{
    namespace lua
    {
        void register_input(lua_State* L);
    }
}
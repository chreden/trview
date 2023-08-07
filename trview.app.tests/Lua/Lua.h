#pragma once

struct lua_State;

namespace trview
{
    namespace tests
    {
        struct LuaState
        {
            LuaState();
            ~LuaState();
            operator lua_State* ();
            lua_State* L;
        };
    }
}


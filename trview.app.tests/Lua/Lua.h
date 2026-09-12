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

        // Helper to call Lua register functions and clean up afterwards
        template <auto register_func, auto clear_func>
        struct reg_scope
        {
            reg_scope(auto&&... args)
            {
                register_func(args...);
            }

            ~reg_scope()
            {
                clear_func();
            }
        };
    }
}


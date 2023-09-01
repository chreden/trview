#pragma once

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

namespace trview
{
    namespace lua
    {
        void imgui_register(lua_State* L);
    }
}

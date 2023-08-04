#include "Lua.h"
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

namespace trview
{
    namespace tests
    {
        LuaState::LuaState()
        {
            L = luaL_newstate();
        }

        LuaState::~LuaState()
        {
            lua_close(L);
        }

        LuaState::operator lua_State* ()
        {
            return L;
        }
    }
}

#include "pch.h"
#include "tables.h"

namespace trview
{
    namespace lua
    {
        std::string get_string(lua_State* L, int index, const std::string& name)
        {
            luaL_checktype(L, index, LUA_TTABLE);
            lua_getfield(L, index, name.c_str());
            luaL_checktype(L, -1, LUA_TSTRING);
            std::string value = lua_tostring(L, -1);
            lua_pop(L, 1);
            return value;
        }

        int get_int(lua_State* L, int index, const std::string& name)
        {
            luaL_checktype(L, index, LUA_TTABLE);
            lua_getfield(L, index, name.c_str());
            luaL_checktype(L, -1, LUA_TNUMBER);
            int value = lua_tointeger(L, -1);
            lua_pop(L, 1);
            return value;
        }
    }
}

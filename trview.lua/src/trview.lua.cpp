// trview.lua.imgui.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"

#include <optional>
#include <string>

#include "../inc/trview.lua.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            /// <summary>
            /// Get an optional bool named field from a table.
            /// </summary>
            /// <param name="L">Lua state.</param>
            /// <param name="index">Index of the table on the stack.</param>
            /// <param name="name">Field name.</param>
            /// <returns>Optional - empty if not set, or boolean.</returns>
            std::optional<bool> get_optional_bool(lua_State* L, int index, const std::string& name)
            {
                luaL_checktype(L, index, LUA_TTABLE);
                lua_getfield(L, index, name.c_str());
                if (lua_isnil(L, -1))
                {
                    lua_pop(L, 1);
                    return std::nullopt;
                }
                luaL_checktype(L, -1, LUA_TBOOLEAN);
                bool value = lua_toboolean(L, -1);
                lua_pop(L, 1);
                return value;
            }

            bool get_bool(lua_State* L, int index, const std::string& name)
            {
                luaL_checktype(L, index, LUA_TTABLE);
                lua_getfield(L, index, name.c_str());
                luaL_checktype(L, -1, LUA_TBOOLEAN);
                bool value = lua_toboolean(L, -1);
                lua_pop(L, 1);
                return value;
            }
        }
    }
}

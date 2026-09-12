#pragma once

namespace trview
{
    namespace lua
    {
        template <typename T>
        std::vector<T> get_list(lua_State* L, int index, const std::string& name)
        {
            luaL_checktype(L, index, LUA_TTABLE);
            lua_getfield(L, index, name.c_str());
            if (lua_isnil(L, -1))
            {
                lua_pop(L, 1);
                return {};
            }

            luaL_checktype(L, -1, LUA_TTABLE);
            const int top = lua_gettop(L);

            std::vector<T> results;
            lua_pushnil(L);
            while (lua_next(L, top) != 0)
            {
                // TODO: Checks for other types.
                results.push_back(*static_cast<T*>(lua_touserdata(L, -1)));
                lua_pop(L, 1);
            }

            lua_pop(L, 1);
            return results;
        }

        template <typename T>
        std::optional<T> get_optional_enum(lua_State* L, int index, const std::string& name)
        {
            const auto value = get_optional_integer(L, index, name);
            if (value == std::nullopt)
            {
                return std::nullopt;
            }
            return static_cast<T>(value.value());
        }
    }
}

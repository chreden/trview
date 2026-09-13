module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

export module trview.lua:Metatables;

import std;

namespace
{
    template <typename T>
    constexpr bool is_shared_ptr_v = false;

    template <typename T>
    constexpr bool is_shared_ptr_v<std::shared_ptr<T>> = true;
}

namespace trview
{
    export namespace lua
    {
        void assign_metatable(lua_State* L, int ref_index)
        {
            if (ref_index == LUA_NOREF)
            {
                throw std::exception("Metatable not a registry index");
            }
            lua_rawgeti(L, LUA_REGISTRYINDEX, ref_index);
            lua_setmetatable(L, -2);
        }

        int store_metatable(lua_State* L, const std::unordered_map<std::string, lua_CFunction>& map)
        {
            lua_newtable(L);
            for (const auto& func : map)
            {
                lua_pushcfunction(L, func.second);
                lua_setfield(L, -2, func.first.c_str());
            }
            return luaL_ref(L, LUA_REGISTRYINDEX);
        }

        void create_metatable(lua_State* L, const std::unordered_map<std::string, lua_CFunction>& map)
        {
            lua_newtable(L);
            for (const auto& func : map)
            {
                lua_pushcfunction(L, func.second);
                lua_setfield(L, -2, func.first.c_str());
            }
            lua_setmetatable(L, -2);
        }

        bool equal_metatable(lua_State* L, int index, int metatable)
        {
            if (0 == lua_getmetatable(L, index))
            {
                return false;
            }
            lua_rawgeti(L, LUA_REGISTRYINDEX, metatable);
            bool equal = lua_compare(L, -2, -1, LUA_OPEQ);
            lua_pop(L, 2);
            return equal;
        }

        using FunctionMap = const std::unordered_map<std::string, lua_CFunction>&;

        template <FunctionMap T>
        int default_index(lua_State* L)
        {
            const std::string key = lua_tostring(L, 2);
            const auto found = T.find(key);
            if (found != T.end())
            {
                return found->second(L);
            }
            return 0;
        }

        template <typename T, auto Prop>
        int prop_getter(lua_State* L)
        {
            const auto& self = get_userdata<T>(L, 1);
            if constexpr (std::is_member_function_pointer_v<decltype(Prop)>)
            {
                if constexpr (is_shared_ptr_v<T>)
                {
                    const auto result = (self.get()->*Prop)();
                    using ResultType = typename std::remove_const<decltype(result)>::type;
                    if constexpr (std::is_same_v<ResultType, float>)
                    {
                        lua_pushnumber(L, result);
                    }
                    else if constexpr (std::is_same_v<ResultType, int>)
                    {
                        lua_pushinteger(L, result);
                    }
                    else if constexpr (std::is_same_v<ResultType, bool>)
                    {
                        lua_pushboolean(L, result);
                    }
                }
                else
                {
                    const auto result = (self.*Prop)();
                    if constexpr (std::is_same_v<decltype(result), float>)
                    {
                        lua_pushnumber(L, result);
                    }
                    else if constexpr (std::is_same_v<decltype(result), int>)
                    {
                        lua_pushinteger(L, result);
                    }
                }
            }
            else
            {
                lua_pushnumber(L, self.*Prop);
            }
            return 1;
        }
    }
}

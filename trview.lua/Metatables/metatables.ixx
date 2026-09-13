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

    template <typename T>
    constexpr bool is_optional_v = false;

    template <typename T>
    constexpr bool is_optional_v<std::optional<T>> = true;

    template <typename T>
    concept is_container = requires (T v)
    {
        v.begin();
        v.end();
    };

    template <typename T>
    int return_result(lua_State* L, T&& result)
    {
        using ResultType = typename std::remove_cvref<decltype(result)>::type;
        if constexpr (is_optional_v<ResultType>)
        {
            if (!result.has_value())
            {
                lua_pushnil(L);
                return 1;
            }
            return return_result(L, result.value());
        }
        else if constexpr (std::is_same_v<ResultType, std::string>)
        {
            lua_pushstring(L, result.c_str());
            return 1;
        }
        else if constexpr (is_container<ResultType>)
        {
            lua_newtable(L);
            int index = 1;
            for (const auto& item : result)
            {
                lua_pushnumber(L, index);
                return_result(L, item);
                lua_settable(L, -3);
                ++index;
            }
            return 1;
        }
        else if constexpr (std::is_same_v<ResultType, float>)
        {
            lua_pushnumber(L, result);
            return 1;
        }
        else if constexpr (std::is_same_v<ResultType, bool>)
        {
            lua_pushboolean(L, result);
            return 1;
        }
        else if constexpr (std::is_integral_v<ResultType>)
        {
            lua_pushinteger(L, result);
            return 1;
        }
        else
        {
            return to_lua(L, result);
        }
    }

    template <typename T>
    constexpr T&& no_transform(T&& V)
    {
        return V;
    }
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

        template <typename T, auto Prop, auto Transform>
        int prop_getter_with_transform(lua_State* L)
        {
            constexpr auto apply_transform = [&](auto&& value)
            {
                if constexpr (Transform != nullptr)
                {
                    return Transform(value);
                }
                else
                {
                    return value;
                }
            };

            const auto& self = get_userdata<T>(L, 1);
            if constexpr (std::is_member_function_pointer_v<decltype(Prop)>)
            {
                if constexpr (is_shared_ptr_v<T>)
                {
                    return return_result(L, apply_transform((self.get()->*Prop)()));
                }
                else
                {
                    return return_result(L, apply_transform((self.*Prop)()));
                }
            }
            else
            {
                if constexpr (is_shared_ptr_v<T>)
                {
                    return return_result(L, apply_transform(self.get()->*Prop));
                }
                else
                {
                    return return_result(L, apply_transform(self.*Prop));
                }
            }
        }

        template <typename T, auto Prop>
        int prop_getter(lua_State* L)
        {
            return prop_getter_with_transform<T, Prop, nullptr>(L);
        }
    }
}

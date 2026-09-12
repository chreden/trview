#pragma once

namespace trview
{
    namespace lua
    {
        template <typename Func>
        int push_list_p(lua_State* L, std::ranges::input_range auto&& range, Func&& func)
        {
            lua_newtable(L);
            int index = 1;
            for (const auto& item : range)
            {
                lua_pushnumber(L, index);
                func(L, item.lock());
                lua_settable(L, -3);
                ++index;
            }
            return 1;
        }

        template <typename Func>
        int push_list(lua_State* L, std::ranges::input_range auto&& range, Func&& func)
        {
            lua_newtable(L);
            int index = 1;
            for (const auto& item : range)
            {
                lua_pushnumber(L, index);
                func(L, item);
                lua_settable(L, -3);
                ++index;
            }
            return 1;
        }

        template <typename T>
        void set_enum_value(lua_State* L, const EnumValue<T>& value)
        {
            lua_pushinteger(L, static_cast<int>(value.value));
            lua_setfield(L, -2, value.name.c_str());
        }

        template <typename T>
        void create_enum(lua_State* L, const std::string& name, const std::vector<EnumValue<T>>& values)
        {
            lua_newtable(L);
            for (const auto& v : values)
            {
                set_enum_value(L, v);
            }
            lua_setfield(L, -2, name.c_str());
        }

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
                const float result = (self.*Prop)();
                lua_pushnumber(L, result);
            }
            else
            {
                lua_pushnumber(L, self.*Prop);
            }
            return 1;
        }
    }
}

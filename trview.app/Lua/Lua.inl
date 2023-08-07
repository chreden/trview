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

        template <typename T>
        int create(lua_State* L, const std::shared_ptr<T>& self, lua_CFunction index, lua_CFunction new_index)
        {
            if (!self)
            {
                lua_pushnil(L);
                return 1;
            }

            set_self(L, self);

            lua_newtable(L);
            lua_pushcfunction(L, index);
            lua_setfield(L, -2, "__index");
            lua_pushcfunction(L, new_index);
            lua_setfield(L, -2, "__newindex");
            lua_pushcfunction(L, gc<T>);
            lua_setfield(L, -2, "__gc");
            lua_setmetatable(L, -2);
            return 1;
        }

        template <typename T>
        std::shared_ptr<T> get_self(lua_State* L, int index)
        {
            luaL_checktype(L, index, LUA_TUSERDATA);
            return *static_cast<std::shared_ptr<T>*>(lua_touserdata(L, index));
        }

        template <typename T>
        T* get_self_raw(lua_State* L, int index)
        {
            luaL_checktype(L, index, LUA_TUSERDATA);
            return *static_cast<T**>(lua_touserdata(L, index));
        }

        template <typename T>
        void set_self(lua_State* L, const std::shared_ptr<T>& self)
        {
            using Ptr = std::shared_ptr<T>;
            auto userdata = static_cast<Ptr*>(lua_newuserdata(L, sizeof(Ptr)));
            new (userdata) Ptr(self);
        }

        template <typename T>
        int gc(lua_State* L)
        {
            luaL_checktype(L, 1, LUA_TUSERDATA);
            auto userdata = static_cast<std::shared_ptr<T>*>(lua_touserdata(L, 1));
            userdata->~shared_ptr<T>();
            return 0;
        }
    }
}

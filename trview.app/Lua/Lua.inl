#pragma once

namespace trview
{
    namespace lua
    {
        template <typename T>
        int push_list(lua_State* L, const std::vector<std::weak_ptr<T>>& range, const std::function<void(lua_State*, const std::shared_ptr<T>&)>& func)
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

        template <typename T>
        int push_list(lua_State* L, const std::vector<T>& range, const std::function<void(lua_State*, const T&)>& func)
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
        T* get_self(lua_State* L)
        {
            luaL_checktype(L, 1, LUA_TUSERDATA);
            return *static_cast<T**>(lua_touserdata(L, 1));
        }
    }
}

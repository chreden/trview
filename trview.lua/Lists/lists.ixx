module;

#include <external/lua/src/lua.h>

export module trview.lua:Lists;

import std;

namespace trview
{
    export namespace lua
    {
        int push_string(lua_State* L, const std::string& text)
        {
            lua_pushstring(L, text.c_str());
            return 1;
        }

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
    }
}

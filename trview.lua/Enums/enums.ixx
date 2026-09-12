module;

#include <external/lua/src/lua.h>

export module trview.lua:Enums;

import std;

namespace trview
{
    export namespace lua
    {
        template <typename T>
        struct EnumValue
        {
            std::string name;
            T value;
        };

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
    }
}
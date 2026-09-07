module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

export module trview.lua:Tables;

import std;

namespace trview
{
    namespace lua
    {
        export double get_double(lua_State* L, int index, const std::string& name);
        export float get_float(lua_State* L, int index, const std::string& name);
        export int get_integer(lua_State* L, int index, const std::string& name);
        export std::optional<double> get_optional_double(lua_State* L, int index, const std::string& name);
        export std::optional<float> get_optional_float(lua_State* L, int index, const std::string& name);
        export std::optional<int> get_optional_integer(lua_State* L, int index, const std::string& name);
        export std::optional<std::string> get_optional_string(lua_State* L, int index, const std::string& name);
        export std::string get_string(lua_State* L, int index, const std::string& name);

        export template <typename T>
        std::vector<T> get_list(lua_State* L, int index, const std::string& name);

        export template <typename T>
        std::optional<T> get_optional_enum(lua_State* L, int index, const std::string& name);

        export struct EnumValue2
        {
            std::string name;
            int value;
        };

        export void set_enum(lua_State* L, const std::string& name, int index, const std::vector<EnumValue2>& values);
        export void set_integer(lua_State* L, int index, const std::string& name, int value);
    }
}

#include "tables.inl"

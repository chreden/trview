#pragma once

#include <optional>
#include <string>

struct lua_State;

namespace trview
{
    namespace lua
    {
        float get_float(lua_State* L, int index, const std::string& name);
        int get_integer(lua_State* L, int index, const std::string& name);
        std::optional<float> get_optional_float(lua_State* L, int index, const std::string& name);
        std::optional<int> get_optional_integer(lua_State* L, int index, const std::string& name);
        std::optional<std::string> get_optional_string(lua_State* L, int index, const std::string& name);
        std::string get_string(lua_State* L, int index, const std::string& name);
    }
}

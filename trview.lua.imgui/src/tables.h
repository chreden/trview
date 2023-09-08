#pragma once

#include <string>

struct lua_State;

namespace trview
{
    namespace lua
    {
        std::string get_string(lua_State* L, int index, const std::string& name);
        int get_int(lua_State* L, int index, const std::string& name);
    }
}

module;

#include <external/lua/src/lua.h>

export module trview.lua.imgui:Input;

namespace trview
{
    namespace lua
    {
        void register_input(lua_State* L);
    }
}
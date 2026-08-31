module;

#include <external/lua/src/lua.h>

export module trview.lua.imgui;

namespace trview
{
    namespace lua
    {
        export void imgui_register(lua_State* L);
    }
}

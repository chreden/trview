module;

#include <external/lua/src/lua.h>

export module trview.app:LuaLight;

import std;
import :ILight;

namespace trview
{
    namespace lua
    {
        export int create_light(lua_State* L, const std::shared_ptr<ILight>& light);
    }
}
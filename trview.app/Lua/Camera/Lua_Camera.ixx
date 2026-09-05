module;

#include <external/lua/src/lua.h>

export module trview.app:LuaCamera;

import std;
import :ICamera;

namespace trview
{
    namespace lua
    {
        export void camera_register(lua_State* L);
        export int create_camera(lua_State* L, const std::shared_ptr<ICamera>& camera);
        export std::shared_ptr<ICamera> to_camera(lua_State* L, int index);
    }
}

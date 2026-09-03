module;

#include <external/lua/src/lua.h>

export module trview.app:LuaCameraSink;

import std;
import :ICameraSink;

namespace trview
{
    namespace lua
    {
        export int create_camera_sink(lua_State* L, std::shared_ptr<ICameraSink> camera_sink);
    }
}

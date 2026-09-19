module;

#include <external/lua/src/lua.h>

export module trview.app:LuaCameraSink;

import std;
import :ICameraSink;

namespace trview
{
    namespace lua
    {
        export void camera_sink_register(lua_State* L);
        export int to_lua(lua_State* L, const std::weak_ptr<ICameraSink>& camera_sink);
        export int to_lua(lua_State* L, ICameraSink::Type type);
    }

    export using lua::to_lua;
}

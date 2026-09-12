module;

#include <external/lua/src/lua.h>

export module trview.app:LuaStaticMesh;

import std;
import :IStaticMesh;

namespace trview
{
    namespace lua
    {
        export void static_mesh_register(lua_State* L);
        export int create_static_mesh(lua_State* L, const std::shared_ptr<IStaticMesh>& mesh);
    }
}

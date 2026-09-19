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
        export int to_lua(lua_State* L, const std::weak_ptr<IStaticMesh>& mesh);
        export int to_lua(lua_State* L, IStaticMesh::Type type);
    }

    export using lua::to_lua;
}

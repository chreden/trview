module;

#include <external/lua/src/lua.h>

export module trview.app:LuaMesh;

import std;

import :IMesh;

namespace trview
{
    export namespace lua
    {
        int create_mesh(lua_State* L, const std::shared_ptr<IMesh>& mesh);
        void mesh_register(lua_State* L, const IMesh::Source& source);
        void mesh_unregister();
    }
}

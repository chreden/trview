#include "Lua_Mesh.h"
#include "../Lua.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            IMesh::Source mesh_source;
        }

        void mesh_register(lua_State* L, const IMesh::Source& source)
        {
            L;
            mesh_source = source;
        }
    }
}
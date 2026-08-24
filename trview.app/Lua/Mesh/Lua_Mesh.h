#pragma once

#include "../../Geometry/IMesh.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_mesh(lua_State* L, const std::shared_ptr<IMesh>& mesh);
        void mesh_register(lua_State* L, const IMesh::Source& source);
    }
}

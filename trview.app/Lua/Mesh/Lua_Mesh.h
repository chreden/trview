#pragma once

#include "../../Geometry/IMesh.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        void mesh_register(lua_State* L, const IMesh::Source& source);
    }
}

#pragma once

#include "../../../Elements/StaticMesh.h"

struct lua_State;

namespace trview
{
    namespace lua
    {
        void create_static_mesh(lua_State* L, const std::shared_ptr<IStaticMesh>& mesh);
    }
}

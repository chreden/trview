#pragma once

#include <SimpleMath.h>

struct lua_State;

namespace trview
{
    namespace lua
    {
        int create_bounding_box(lua_State* L, const DirectX::BoundingBox& box);
    }
}

module;

#include "pch.h"

export module trview.app:LuaBoundingBox;

namespace trview
{
    namespace lua
    {
        export int create_bounding_box(lua_State* L, const DirectX::BoundingBox& box);
    }
}

module;

#include <external/lua/src/lua.h>
#include <SimpleMath.h>

module trview.app:LuaBoundingBox;

import :LuaVector3;

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        int create_bounding_box(lua_State* L, const DirectX::BoundingBox& box)
        {
            lua_newtable(L);
            create_vector3(L, box.Center - box.Extents);
            lua_setfield(L, -2, "min");
            create_vector3(L, Vector3(box.Center) + box.Extents);
            lua_setfield(L, -2, "max");
            return 1;
        }
    }
}


#include "BoundingBox.h"
#include "Lua.h"
#include "Vector3.h"

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


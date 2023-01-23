#include "Vector3.h"
#include "Lua.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        int create_vector3(lua_State* L, const Vector3& value)
        {
            lua_newtable(L);
            lua_pushnumber(L, value.x);
            lua_setfield(L, -2, "x");
            lua_pushnumber(L, value.y);
            lua_setfield(L, -2, "y");
            lua_pushnumber(L, value.z);
            lua_setfield(L, -2, "z");
            return 1;
        }
    }
}

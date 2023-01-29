#include "Color.h"
#include "Lua.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        int create_color(lua_State* L, const Color& value)
        {
            lua_newtable(L);
            lua_pushnumber(L, value.R());
            lua_setfield(L, -2, "r");
            lua_pushnumber(L, value.G());
            lua_setfield(L, -2, "g");
            lua_pushnumber(L, value.B());
            lua_setfield(L, -2, "b");
            lua_pushnumber(L, value.A());
            lua_setfield(L, -2, "a");
            return 1;
        }
    }
}

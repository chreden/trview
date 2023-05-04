#include "Colour.h"
#include "Lua.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        int create_colour(lua_State* L, const Color& value)
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

        Colour to_colour(lua_State* L, int index)
        {
            lua_getfield(L, 1, "r");
            float r = static_cast<float>(lua_tonumber(L, -1));
            lua_getfield(L, 1, "g");
            float g = static_cast<float>(lua_tonumber(L, -1));
            lua_getfield(L, 1, "b");
            float b = static_cast<float>(lua_tonumber(L, -1));
            lua_getfield(L, 1, "a");
            float a = static_cast<float>(lua_tonumber(L, -1));
            return Colour(a, r, g, b);
        }
    }
}

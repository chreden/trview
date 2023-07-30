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
            const int field_index = index < 0 ? (index + lua_gettop(L) + 1) : index;
            lua_getfield(L, field_index, "r");
            float r = static_cast<float>(lua_tonumber(L, -1));
            lua_getfield(L, field_index, "g");
            float g = static_cast<float>(lua_tonumber(L, -1));
            lua_getfield(L, field_index, "b");
            float b = static_cast<float>(lua_tonumber(L, -1));
            lua_getfield(L, field_index, "a");
            float a = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 4);
            return Colour(a, r, g, b);
        }

        int colour_new(lua_State* L)
        {
            float r = static_cast<float>(lua_tonumber(L, 1));
            float g = static_cast<float>(lua_tonumber(L, 2));
            float b = static_cast<float>(lua_tonumber(L, 3));
            float a = lua_gettop(L) >= 4 ? static_cast<float>(lua_tonumber(L, 4)) : 1.0f;
            return create_colour(L, Colour(a, r, g, b));
        }

        void colour_register(lua_State* L)
        {
            lua_newtable(L);
            lua_pushcfunction(L, colour_new);
            lua_setfield(L, -2, "new");
            lua_setglobal(L, "Colour");
        }
    }
}

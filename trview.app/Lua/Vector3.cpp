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

        Vector3 to_vector3(lua_State* L, int index)
        {
            lua_getfield(L, index, "x");
            const float x = static_cast<float>(lua_tonumber(L, -1));
            lua_getfield(L, index, "y");
            const float y = static_cast<float>(lua_tonumber(L, -1));
            lua_getfield(L, index, "z");
            const float z = static_cast<float>(lua_tonumber(L, -1));
            lua_pop(L, 3);
            return Vector3(x, y, z);
        }

        Vector3 to_vector3(lua_State* L, int index, const std::string& name, const Vector3& default_value)
        {
            if (LUA_TTABLE == lua_getfield(L, index, name.c_str()))
            {
                int field_index = lua_gettop(L);
                lua_getfield(L, field_index, "x");
                const float x = static_cast<float>(lua_tonumber(L, -1));
                lua_getfield(L, field_index, "y");
                const float y = static_cast<float>(lua_tonumber(L, -1));
                lua_getfield(L, field_index, "z");
                const float z = static_cast<float>(lua_tonumber(L, -1));
                lua_pop(L, 4);
                return Vector3(x, y, z);
            }
            else
            {
                lua_pop(L, 1);
            }

            return default_value;
        }
    }
}

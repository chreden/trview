#include "Vector3.h"
#include "Lua.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int vector3_new(lua_State* L)
            {
                int args = lua_gettop(L);
                switch (args)
                {
                case 0:
                    return create_vector3(L, Vector3());
                case 3:
                    return create_vector3(L,
                        Vector3(
                            static_cast<float>(lua_tonumber(L, 1)),
                            static_cast<float>(lua_tonumber(L, 2)),
                            static_cast<float>(lua_tonumber(L, 3))));
                }
                return luaL_error(L, "Vector3.new expects 0 or 3 number arguments");
            }
        }

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
            const int field_index = index + lua_gettop(L) + 1;
            lua_getfield(L, field_index, "x");
            const float x = static_cast<float>(lua_tonumber(L, -1));
            lua_getfield(L, field_index, "y");
            const float y = static_cast<float>(lua_tonumber(L, -1));
            lua_getfield(L, field_index, "z");
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

        void vector3_register(lua_State* L)
        {
            lua_newtable(L);
            lua_pushcfunction(L, vector3_new);
            lua_setfield(L, -2, "new");
            lua_setglobal(L, "Vector3");
        }
    }
}

#include "Vector2.h"
#include "Lua.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace lua
    {
        namespace
        {
            int vector2_metatable = LUA_NOREF;

            int vector2_constructor(lua_State* L)
            {
                int args = lua_gettop(L);
                switch (args)
                {
                case 1:
                    return create_vector2(L, Vector2());
                case 3:
                    return create_vector2(L,
                        Vector2(
                            static_cast<float>(lua_tonumber(L, 2)),
                            static_cast<float>(lua_tonumber(L, 3))));
                }
                return luaL_error(L, "Vector3 constructor expects 0 or 2 number arguments");
            }

            int vector2_index(lua_State* L)
            {
                const auto& self = get_userdata<Vector2>(L, 1);
                const std::string key = lua_tostring(L, 2);
                if (key == "x")
                {
                    lua_pushnumber(L, self.x);
                    return 1;
                }
                else if (key == "y")
                {
                    lua_pushnumber(L, self.y);
                    return 1;
                }
                return 0;
            }
        }

        int create_vector2(lua_State* L, const Vector2& value)
        {
            create_userdata(L, value);
            assign_metatable(L, vector2_metatable);
            return 1;
        }

        void vector2_register(lua_State* L)
        {
            vector2_metatable = store_metatable(L,
                {
                    { "__index", vector2_index },
                    { "__gc", default_gc<Vector2> }
                });

            lua_newtable(L);
            create_metatable(L,
                {
                    { "__call", vector2_constructor }
                });
            lua_setglobal(L, "Vector2");
        }
    }
}

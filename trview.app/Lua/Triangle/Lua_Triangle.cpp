#include "Lua_Triangle.h"
#include "../Lua.h"

namespace trview
{
    namespace lua
    {
        namespace
        {
            int triangle_metatable = LUA_NOREF;

            int triangle_new(lua_State* L)
            {
                L;
                return 0;
            }

            int triangle_index(lua_State* L)
            {
                L;
                return 0;
            }

            int triangle_newindex(lua_State* L)
            {
                L;
                return 0;
            }

            int triangle_gc(lua_State* L)
            {
                cleanup_userdata<Triangle>(L, 1);
                return 0;
            }

            int triangle_constructor(lua_State* L)
            {
                return create_triangle(L, Triangle{});
            }
        }

        int create_triangle(lua_State* L, const Triangle& triangle)
        {
            create_userdata(L, triangle);
            assign_metatable(L, triangle_metatable);
            return 1;
        }

        void triangle_register(lua_State* L)
        {
            triangle_metatable = store_metatable(L,
                {
                    { "__index", triangle_index },
                    { "__newindex", triangle_newindex },
                    { "__gc", triangle_gc }
                });

            lua_newtable(L);
            create_metatable(L,
                {
                    { "__call", triangle_constructor }
                });
            lua_pushcfunction(L, triangle_new);
            lua_setfield(L, -2, "new");
            lua_setglobal(L, "Triangle");
        }
    }
}
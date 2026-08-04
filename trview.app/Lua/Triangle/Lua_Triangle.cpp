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
                const auto& self = get_userdata<Triangle>(L, 1);
                const std::string key = lua_tostring(L, 2);

                if (key == "animation_mode")
                {
                    lua_pushinteger(L, static_cast<int>(self.animation_mode));
                    return 1;
                }
                else if (key == "collision_mode")
                {
                    lua_pushinteger(L, static_cast<int>(self.collision_mode));
                    return 1;
                }
                else if (key == "colours")
                {
                }
                else if (key == "current_frame")
                {
                    lua_pushinteger(L, self.current_frame);
                    return 1;
                }
                else if (key == "current_time")
                {
                    lua_pushnumber(L, self.current_time);
                    return 1;
                }
                else if (key == "frames")
                {
                }
                else if (key == "frame_time")
                {
                    lua_pushnumber(L, self.frame_time);
                    return 1;
                }
                else if (key == "normal")
                {
                }
                else if (key == "normals")
                {
                }
                else if (key == "position")
                {
                }
                else if (key == "side_mode")
                {
                }
                else if (key == "texture")
                {
                }
                else if (key == "texture_mode")
                {
                }
                else if (key == "transform")
                {
                }
                else if (key == "transparency_mode")
                {
                }
                else if (key == "uv")
                {
                }
                else if (key == "vertices")
                {
                }

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
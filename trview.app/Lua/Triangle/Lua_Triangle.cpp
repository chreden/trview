#include "Lua_Triangle.h"
#include "../Lua.h"
#include "../Vector3.h"
#include "../Colour.h"
#include <trview.lua/inc/tables.h>

namespace trview
{
    namespace lua
    {
        namespace
        {
            int triangle_metatable = LUA_NOREF;

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
                    return push_list(L, self.colours, create_colour);
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
                    return create_vector3(L, self.normal());
                }
                else if (key == "normals")
                {
                    return push_list(L, self.normals, create_vector3);
                }
                else if (key == "position")
                {
                    return create_vector3(L, self.position());
                }
                else if (key == "side_mode")
                {
                    lua_pushinteger(L, static_cast<int>(self.side_mode));
                    return 1;
                }
                else if (key == "texture")
                {
                    lua_pushinteger(L, self.texture());
                    return 1;
                }
                else if (key == "texture_mode")
                {
                    lua_pushinteger(L, static_cast<int>(self.texture_mode));
                    return 1;
                }
                else if (key == "transform")
                {

                }
                else if (key == "transparency_mode")
                {
                    lua_pushinteger(L, static_cast<int>(self.transparency_mode));
                    return 1;
                }
                else if (key == "uv")
                {
                    
                }
                else if (key == "vertices")
                {
                    return push_list(L, self.vertices, create_vector3);
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
                const Triangle::AnimationMode animation_mode =
                    static_cast<Triangle::AnimationMode>(get_optional_integer(L, 2, "animation_mode").value_or(static_cast<int>(Triangle::AnimationMode::None)));
                return create_triangle(L, Triangle{ .animation_mode = animation_mode });
            }

            int triangle_class_index(lua_State* L)
            {
                const std::string key = lua_tostring(L, 2);
                if (key == "new")
                {
                    lua_pushcfunction(L, triangle_constructor);
                    return 1;
                }
                return 0;
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
            create_enum<Triangle::AnimationMode>(L, "AnimationMode",
                {
                    { "None", Triangle::AnimationMode::None },
                    { "Swap", Triangle::AnimationMode::Swap },
                    { "UV", Triangle::AnimationMode::UV }
                });
            create_enum<Triangle::CollisionMode>(L, "CollisionMode",
                {
                    { "Disabled", Triangle::CollisionMode::Disabled },
                    { "Enabled", Triangle::CollisionMode::Enabled }
                });
            create_enum<Triangle::SideMode>(L, "SideMode",
                {
                    { "Single", Triangle::SideMode::Single },
                    { "Double", Triangle::SideMode::Double }
                });
            create_enum<Triangle::TextureMode>(L, "TextureMode",
                {
                    { "Textured", Triangle::TextureMode::Textured },
                    { "Untextured", Triangle::TextureMode::Untextured }
                });
            create_enum<Triangle::TransparencyMode>(L, "TransparencyMode",
                {
                    { "None", Triangle::TransparencyMode::None },
                    { "Normal", Triangle::TransparencyMode::Normal },
                    { "Additive", Triangle::TransparencyMode::Additive }
                });
            create_metatable(L,
                {
                    { "__call", triangle_constructor },
                    { "__index", triangle_class_index }
                });
            lua_setglobal(L, "Triangle");
        }
    }
}
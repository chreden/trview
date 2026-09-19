module;

#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <SimpleMath.h>

module trview.app:LuaTriangle;

import trview.lua;

import :Lua;
import :LuaColour;
import :LuaVector3;

using namespace DirectX::SimpleMath;

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

                self;

                if (key == "frames")
                {
                    lua_pushnil(L);
                    return 1;
                }
                else if (key == "transform")
                {
                }
                else if (key == "uv")
                {
                }

                return 0;
            }

            int triangle_newindex(lua_State* L)
            {
                L;
                return 0;
            }

            int triangle_constructor(lua_State* L)
            {
                const Triangle::AnimationMode animation_mode = get_optional_enum<Triangle::AnimationMode>(L, 2, "animation_mode").value_or(Triangle::AnimationMode::None);
                const Triangle::CollisionMode collision_mode = get_optional_enum<Triangle::CollisionMode>(L, 2, "collision_mode").value_or(Triangle::CollisionMode::Disabled);
                const std::vector<Colour> colours = get_list<Colour>(L, 2, "colours");
                const uint32_t current_frame = static_cast<uint32_t>(get_optional_integer(L, 2, "current_frame").value_or(0));
                const float current_time = get_optional_float(L, 2, "current_time").value_or(0.0f);
                const float frame_time = get_optional_float(L, 2, "frame_time").value_or(0.0f);
                const std::vector<Vector3> normals = get_list<Vector3>(L, 2, "normals");
                const Triangle::SideMode side_mode = get_optional_enum<Triangle::SideMode>(L, 2, "side_mode").value_or(Triangle::SideMode::Single);
                const Triangle::TextureMode texture_mode = get_optional_enum<Triangle::TextureMode>(L, 2, "texture_mode").value_or(Triangle::TextureMode::Untextured);
                const Triangle::TransparencyMode transparency_mode = get_optional_enum<Triangle::TransparencyMode>(L, 2, "transparency_mode").value_or(Triangle::TransparencyMode::None);
                const std::vector<DirectX::SimpleMath::Vector3> vertices = get_list<DirectX::SimpleMath::Vector3>(L, 2, "vertices");

                // TODO: Length check on vectors

                return create_triangle(L,
                    Triangle
                    {
                        .animation_mode = animation_mode,
                        .collision_mode = collision_mode,
                        .colours = { static_cast<Color>(colours[0]), static_cast<Color>(colours[1]), static_cast<Color>(colours[2]) },
                        .current_frame = current_frame,
                        .current_time = current_time,
                        // frames
                        .frame_time = frame_time,
                        .normals = { normals[0], normals[1], normals[2] },
                        .side_mode = side_mode,
                        .texture_mode = texture_mode,
                        .transparency_mode = transparency_mode,
                        .vertices = { vertices[0], vertices[1], vertices[2] }
                    });
            }

            const std::unordered_map<std::string, lua_CFunction> Functions
            {
                { "animation_mode", prop_getter<Triangle, &Triangle::animation_mode> },
                { "collision_mode", prop_getter<Triangle, &Triangle::collision_mode> },
                { "colours", prop_getter<Triangle, &Triangle::colours> },
                { "current_frame", prop_getter<Triangle, &Triangle::current_frame> },
                { "current_time", prop_getter<Triangle, &Triangle::current_time> },
                { "frame_time", prop_getter<Triangle, &Triangle::frame_time> },
                { "normal", prop_getter<Triangle, &Triangle::normal> },
                { "normals", prop_getter<Triangle, &Triangle::normals> },
                { "position", prop_getter<Triangle, &Triangle::position> },
                { "side_mode", prop_getter<Triangle, &Triangle::side_mode> },
                { "texture", prop_getter<Triangle, &Triangle::texture> },
                { "texture_mode", prop_getter<Triangle, &Triangle::texture_mode> },
                { "transparency_mode", prop_getter<Triangle, &Triangle::transparency_mode> },
                { "vertices", prop_getter<Triangle, &Triangle::vertices> }
            };
        }

        int create_triangle(lua_State* L, const Triangle& triangle)
        {
            return create_userdata(L, triangle, triangle_metatable);
        }

        int to_lua(lua_State* L, Triangle::AnimationMode mode)
        {
            lua_pushnumber(L, static_cast<int>(mode));
            return 1;
        }

        int to_lua(lua_State* L, Triangle::CollisionMode mode)
        {
            lua_pushnumber(L, static_cast<int>(mode));
            return 1;
        }

        int to_lua(lua_State* L, Triangle::TextureMode mode)
        {
            lua_pushnumber(L, static_cast<int>(mode));
            return 1;
        }

        int to_lua(lua_State* L, Triangle::TransparencyMode mode)
        {
            lua_pushnumber(L, static_cast<int>(mode));
            return 1;
        }

        int to_lua(lua_State* L, Triangle::SideMode mode)
        {
            lua_pushnumber(L, static_cast<int>(mode));
            return 1;
        }

        void triangle_register(lua_State* L)
        {
            triangle_metatable = store_metatable(L,
                {
                    { "__index", default_index<Functions, triangle_index> },
                    { "__newindex", triangle_newindex },
                    { "__gc", default_gc<Triangle> }
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
                    { "__call", triangle_constructor }
                });
            lua_setglobal(L, "Triangle");
        }
    }
}
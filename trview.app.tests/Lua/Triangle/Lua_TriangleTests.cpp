#include <trview.app/Lua/Triangle/Lua_Triangle.h>
#include <trview.app/Lua/Vector3.h>
#include <trview.app/Lua/Lua.h>
#include <trview.app/Lua/Colour.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include "../Lua.h"

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;

using namespace DirectX::SimpleMath;

TEST(Lua_Triangle, Constructor)
{
    LuaState L;
    lua::colour_register(L);
    lua::vector3_register(L);
    lua::triangle_register(L);

    Triangle expected
    {
        .animation_mode = Triangle::AnimationMode::Swap,
        .collision_mode = Triangle::CollisionMode::Enabled,
        .colours = { Color(1, 1, 1), Color(1, 1, 1), Color(1, 1, 1) },
        .current_frame = 0,
        .current_time = 1.0f,
        // frames
        .frame_time = 1.0f,
        .normals = { Vector3::Down, Vector3::Down, Vector3::Down },
        .side_mode = Triangle::SideMode::Double,
        .texture_mode = Triangle::TextureMode::Untextured,
        .transparency_mode = Triangle::TransparencyMode::Normal,
        .vertices = { Vector3(0,1,0), Vector3(1,0,0), Vector3(0,0,1) }
    };

    ASSERT_EQ(0, luaL_dostring(L, "return Triangle( { animation_mode = Triangle.AnimationMode.Swap, collision_mode = Triangle.CollisionMode.Enabled, colours = { Colour(1, 1, 1), Colour(1, 1, 1), Colour(1, 1, 1) }, current_frame = 0, current_time = 1.0, frame_time = 1.0, normals = { Vector3(0, -1, 0), Vector3(0, -1, 0), Vector3(0, -1, 0) }, side_mode = Triangle.SideMode.Double, texture_mode = Triangle.TextureMode.Untextured, transparency_mode = Triangle.TransparencyMode.Normal, vertices = { Vector3(0,1,0), Vector3(1,0,0), Vector3(0,0,1) } } )"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<Triangle>(L, -1), expected);
}

TEST(Lua_Triangle, AnimationMode)
{
    LuaState L;
    lua::triangle_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.AnimationMode.None"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.AnimationMode.Swap"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.AnimationMode.UV"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
}

TEST(Lua_Triangle, CollisionMode)
{
    LuaState L;
    lua::triangle_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.CollisionMode.Disabled"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.CollisionMode.Enabled"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
}

TEST(Lua_Triangle, SideMode)
{
    LuaState L;
    lua::triangle_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.SideMode.Single"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.SideMode.Double"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
}

TEST(Lua_Triangle, TextureMode)
{
    LuaState L;
    lua::triangle_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.TextureMode.Textured"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.TextureMode.Untextured"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
}

TEST(Lua_Triangle, TransparencyMode)
{
    LuaState L;
    lua::triangle_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.TransparencyMode.None"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.TransparencyMode.Normal"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return Triangle.TransparencyMode.Additive"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
}
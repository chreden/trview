#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include "Lua.h"

#include <SimpleMath.h>

import trview.app;
import trview.tests.common;

using namespace trview;
using namespace trview::tests;
using namespace testing;
using namespace DirectX::SimpleMath;

TEST(Lua_Matrix, Constructor)
{
    LuaState L;
    lua::matrix_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Matrix()"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<Matrix>(L, -1), Matrix::Identity);
}

TEST(Lua_Matrix, Multiply)
{
    LuaState L;
    lua::matrix_register(L);

    const Matrix rotation =
        Matrix::CreateRotationX(3) *
        Matrix::CreateRotationY(2);

    ASSERT_EQ(0, luaL_dostring(L, "return Matrix.rotationX(3) * Matrix.rotationY(2)"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<Matrix>(L, -1), rotation);
}

TEST(Lua_Matrix, RotationX)
{
    LuaState L;
    lua::matrix_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Matrix.rotationX(3)"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<Matrix>(L, -1), Matrix::CreateRotationX(3));
}

TEST(Lua_Matrix, RotationY)
{
    LuaState L;
    lua::matrix_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Matrix.rotationY(3)"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<Matrix>(L, -1), Matrix::CreateRotationY(3));
}

TEST(Lua_Matrix, RotationZ)
{
    LuaState L;
    lua::matrix_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Matrix.rotationZ(3)"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<Matrix>(L, -1), Matrix::CreateRotationZ(3));
}

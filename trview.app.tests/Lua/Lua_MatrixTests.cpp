#include <trview.app/Lua/Lua_Matrix.h>
#include <trview.app/lua/Lua.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include "Lua.h"

using namespace trview;
using namespace trview::tests;
using namespace testing;

TEST(Matrix, Constructor)
{
    LuaState L;
    lua::matrix_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Matrix()"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<DirectX::SimpleMath::Matrix>(L, -1), DirectX::SimpleMath::Matrix::Identity);
}

TEST(Matrix, Multiply)
{
    LuaState L;
    lua::matrix_register(L);

    const DirectX::SimpleMath::Matrix rotation =
        DirectX::SimpleMath::Matrix::CreateRotationX(3) *
        DirectX::SimpleMath::Matrix::CreateRotationY(2);

    ASSERT_EQ(0, luaL_dostring(L, "return Matrix.rotationX(3) * Matrix.rotationY(2)"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<DirectX::SimpleMath::Matrix>(L, -1), rotation);
}

TEST(Matrix, RotationX)
{
    LuaState L;
    lua::matrix_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Matrix.rotationX(3)"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<DirectX::SimpleMath::Matrix>(L, -1), DirectX::SimpleMath::Matrix::CreateRotationX(3));
}

TEST(Matrix, RotationY)
{
    LuaState L;
    lua::matrix_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Matrix.rotationY(3)"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<DirectX::SimpleMath::Matrix>(L, -1), DirectX::SimpleMath::Matrix::CreateRotationY(3));
}

TEST(Matrix, RotationZ)
{
    LuaState L;
    lua::matrix_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return Matrix.rotationZ(3)"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(lua::get_userdata<DirectX::SimpleMath::Matrix>(L, -1), DirectX::SimpleMath::Matrix::CreateRotationZ(3));
}

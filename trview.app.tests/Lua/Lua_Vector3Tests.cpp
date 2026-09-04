#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include "Lua.h"

import trview.app;
import trview.tests.common;

using namespace trview;
using namespace trview::tests;
using namespace testing;

TEST(Lua_Vector3, Constructor)
{
    LuaState L;
    lua::vector3_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "x = Vector3() return x"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.0f, lua_tonumber(L, -1));
}

TEST(Lua_Vector3, ConstructorXYZ)
{
    LuaState L;
    lua::vector3_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "x = Vector3(1, 0.5, 0.25) return x"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.5f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.25f, lua_tonumber(L, -1));
}

TEST(Lua_Vector3, New)
{
    LuaState L;
    lua::vector3_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "x = Vector3.new() return x"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.0f, lua_tonumber(L, -1));
}

TEST(Lua_Vector3, NewXYZ)
{
    LuaState L;
    lua::vector3_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "x = Vector3.new(1, 0.5, 0.25) return x"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.5f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.25f, lua_tonumber(L, -1));
}

TEST(Lua_Vector3, Length)
{
    LuaState L;
    lua::vector3_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "x = Vector3(1, 2, 3) return x"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.length"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_FLOAT_EQ(3.7416575f, static_cast<float>(lua_tonumber(L, -1)));
}

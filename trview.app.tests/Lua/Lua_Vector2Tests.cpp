#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include "Lua.h"

import trview.app;
import trview.tests.common;

using namespace trview;
using namespace trview::tests;
using namespace testing;

TEST(Lua_Vector2, Constructor)
{
    LuaState L;
    lua::vector2_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "x = Vector2() return x"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.0f, lua_tonumber(L, -1));
}

TEST(Lua_Vector2, ConstructorXY)
{
    LuaState L;
    lua::vector2_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "x = Vector2(1, 0.5) return x"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.5f, lua_tonumber(L, -1));
}

TEST(Lua_Vector2, Length)
{
    LuaState L;
    lua::vector2_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "x = Vector2(1, 2) return x"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.length"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_FLOAT_EQ(2.236068f, static_cast<float>(lua_tonumber(L, -1)));
}

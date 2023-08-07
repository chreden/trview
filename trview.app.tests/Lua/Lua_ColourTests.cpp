#include <trview.app/Lua/Colour.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include "Lua.h"

using namespace trview;
using namespace trview::tests;

TEST(Lua_Colour, New)
{
    LuaState L;
    lua::colour_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "x = Colour.new(1, 0.5, 0.25) return x"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.a"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.r"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.g"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.5f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.b"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.25f, lua_tonumber(L, -1));
}

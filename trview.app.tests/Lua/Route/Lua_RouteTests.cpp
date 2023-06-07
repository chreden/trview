#include <trview.app/Lua/Route/Lua_Route.h>
#include <trview.app/Lua/Colour.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;


TEST(Lua_Route, Add)
{
    FAIL();
}

TEST(Lua_Route, Clear)
{
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, clear).Times(1);

    lua_State* L = luaL_newstate();
    lua::create_route(L, route);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "r:clear()"));
}

TEST(Lua_Route, Colour)
{
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, colour).Times(1).WillOnce(Return(Colour(1, 2, 3, 4)));

    lua_State* L = luaL_newstate();
    lua::create_route(L, route);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "x = r.colour return x"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.a"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.r"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.g"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(3.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.b"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(4.0f, lua_tonumber(L, -1));
}

TEST(Lua_Route, Level)
{
    FAIL();
}

TEST(Lua_Route, New)
{
    FAIL();
}

TEST(Lua_Route, Remove)
{
    FAIL();
}

TEST(Lua_Route, SetColour)
{
    Colour called_colour;
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, set_colour).Times(1).WillRepeatedly(SaveArg<0>(&called_colour));

    lua_State* L = luaL_newstate();
    lua::colour_register(L);
    lua::create_route(L, route);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "r.colour = Colour.new(1, 2, 3)"));

    ASSERT_FLOAT_EQ(called_colour.r, 1.0f);
    ASSERT_FLOAT_EQ(called_colour.g, 2.0f);
    ASSERT_FLOAT_EQ(called_colour.b, 3.0f);
    ASSERT_FLOAT_EQ(called_colour.a, 1.0f);
}

TEST(Lua_Route, SetLevel)
{
    FAIL();
}

TEST(Lua_Route, SetWaypointColour)
{
    Colour called_colour;
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, set_waypoint_colour).Times(1).WillRepeatedly(SaveArg<0>(&called_colour));

    lua_State* L = luaL_newstate();
    lua::colour_register(L);
    lua::create_route(L, route);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "r.waypoint_colour = Colour.new(1, 2, 3)"));

    ASSERT_FLOAT_EQ(called_colour.r, 1.0f);
    ASSERT_FLOAT_EQ(called_colour.g, 2.0f);
    ASSERT_FLOAT_EQ(called_colour.b, 3.0f);
    ASSERT_FLOAT_EQ(called_colour.a, 1.0f);
}

TEST(Lua_Route, SetWaypoints)
{
    FAIL();
}

TEST(Lua_Route, WaypointColour)
{
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, waypoint_colour).Times(1).WillOnce(Return(Colour(1, 2, 3, 4)));

    lua_State* L = luaL_newstate();
    lua::create_route(L, route);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "x = r.waypoint_colour return x"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.a"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.r"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.g"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(3.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return x.b"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(4.0f, lua_tonumber(L, -1));
}

TEST(Lua_Route, Waypoints)
{
    FAIL();
}

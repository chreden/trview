#include <trview.app/Lua/Route/Lua_Waypoint.h>
#include <trview.app/Lua/Colour.h>
#include <trview.app/Lua/Vector3.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;
using namespace DirectX::SimpleMath;

TEST(Lua_Waypoint, Colour)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, route_colour).WillRepeatedly(Return(Colour(1, 0.5f, 0.25f)));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "return w.colour"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.colour.r"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.colour.g"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.5f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.colour.b"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.25f, lua_tonumber(L, -1));
}

TEST(Lua_Waypoint, Item)
{
    FAIL();
}

TEST(Lua_Waypoint, Normal)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, normal).WillRepeatedly(Return(Vector3(1, 2, 3)));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "return w.normal"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.normal.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.normal.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.normal.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(3.0f, lua_tonumber(L, -1));
}

TEST(Lua_Waypoint, NewItem)
{
    FAIL();
}

TEST(Lua_Waypoint, NewPosition)
{
    FAIL();
}

TEST(Lua_Waypoint, NewTrigger)
{
    FAIL();
}

TEST(Lua_Waypoint, Notes)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, notes).WillOnce(Return("These are the notes"));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "return w.notes"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("These are the notes", lua_tostring(L, -1));
}

TEST(Lua_Waypoint, Position)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, position).WillRepeatedly(Return(Vector3(1, 2, 3)));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "return w.position"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.position.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1024.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.position.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2048.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.position.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(3072.0f, lua_tonumber(L, -1));
}

TEST(Lua_Waypoint, RandomizerSettings)
{
    FAIL();
}

TEST(Lua_Waypoint, RoomNumber)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, room).WillOnce(Return(123));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "return w.room_number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Waypoint, SetColour)
{
    FAIL();
}

TEST(Lua_Waypoint, SetItem)
{
    FAIL();
}

TEST(Lua_Waypoint, SetNormal)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, set_normal(Vector3(1, 2, 3)));

    lua_State* L = luaL_newstate();
    lua::vector3_register(L);
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "w.normal = Vector3.new(1, 2, 3)"));
}

TEST(Lua_Waypoint, SetNotes)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, set_notes("New notes"));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "w.notes = \"New notes\""));
}

TEST(Lua_Waypoint, SetPosition)
{
    FAIL();
}

TEST(Lua_Waypoint, SetRandomizerSettings)
{
    FAIL();
}

TEST(Lua_Waypoint, SetRoomNumber)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, set_room_number(100));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "w.room_number = 100"));
}

TEST(Lua_Waypoint, SetTrigger)
{
    FAIL();
}

TEST(Lua_Waypoint, SetWaypointColour)
{
    FAIL();
}

TEST(Lua_Waypoint, Trigger)
{
    FAIL();
}

TEST(Lua_Waypoint, Type)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, type).WillOnce(Return(IWaypoint::Type::Trigger));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "return w.type"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Trigger", lua_tostring(L, -1));
}

TEST(Lua_Waypoint, WaypointColour)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, waypoint_colour).WillRepeatedly(Return(Colour(1, 0.5f, 0.25f)));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "return w.waypoint_colour"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.waypoint_colour.r"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1.0f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.waypoint_colour.g"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.5f, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.waypoint_colour.b"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0.25f, lua_tonumber(L, -1));
}

#include <trview.app/Lua/Route/Lua_Waypoint.h>
#include <trview.app/Lua/Colour.h>
#include <trview.app/Lua/Vector3.h>
#include <trview.app/Lua/Elements/Item/Lua_Item.h>
#include <trview.app/Lua/Elements/Sector/Lua_Sector.h>
#include <trview.app/Lua/Elements/Trigger/Lua_Trigger.h>
#include <trview.app/Lua/Elements/Room/Lua_Room.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/IItem.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Routing/IWaypoint.h>
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
    auto item = mock_shared<MockItem>();

    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, item).WillOnce(Return(item));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "return w.item"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
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
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, set_item).Times(1);

    auto item = mock_shared<MockItem>();

    lua_State* L = luaL_newstate();
    lua::waypoint_register(L, [=](auto&&...) { return waypoint; });
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "w = Waypoint.new({item=i})"));
    ASSERT_EQ(0, luaL_dostring(L, "return w"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
}

TEST(Lua_Waypoint, NewPosition)
{
    auto waypoint = mock_shared<MockWaypoint>();
    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, number);

    std::optional<Vector3> position;

    lua_State* L = luaL_newstate();
    lua::vector3_register(L);
    lua::create_room(L, room);
    lua_setglobal(L, "r");
    lua::waypoint_register(L, [&](auto&& pos, auto&&...) { position = pos; return waypoint; });

    ASSERT_EQ(0, luaL_dostring(L, "w = Waypoint.new({position=Vector3.new(1024, 2048, 3072), room=r})"));
    ASSERT_EQ(0, luaL_dostring(L, "return w"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));

    ASSERT_TRUE(position);
    ASSERT_EQ(position.value(), Vector3(1, 2, 3));
}

TEST(Lua_Waypoint, NewSector)
{
    auto waypoint = mock_shared<MockWaypoint>();
    auto room = mock_shared<MockRoom>();

    auto sector = mock_shared<MockSector>();
    EXPECT_CALL(*sector, room).WillRepeatedly(Return(room));
    EXPECT_CALL(*room, sector_centroid).WillRepeatedly(Return(Vector3(1, 2, 3)));

    std::optional<Vector3> position;

    lua_State* L = luaL_newstate();
    lua::vector3_register(L);
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");
    lua::waypoint_register(L, [&](auto&& pos, auto&&...) { position = pos; return waypoint; });

    ASSERT_EQ(0, luaL_dostring(L, "w = Waypoint.new({sector=s})"));
    ASSERT_EQ(0, luaL_dostring(L, "return w"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));

    ASSERT_TRUE(position);
    ASSERT_EQ(position.value(), Vector3(1, 2, 3));
}

TEST(Lua_Waypoint, NewTrigger)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, set_item).Times(1);

    auto trigger = mock_shared<MockTrigger>();

    lua_State* L = luaL_newstate();
    lua::waypoint_register(L, [=](auto&&...) { return waypoint; });
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "w = Waypoint.new({trigger=t})"));
    ASSERT_EQ(0, luaL_dostring(L, "return w"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
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
    UserSettings settings;
    settings.randomizer.settings["test"] =
    {
        .type = RandomizerSettings::Setting::Type::String,
    };
    settings.randomizer.settings["test2"] =
    {
        .type = RandomizerSettings::Setting::Type::Number,
        .default_value = 123.0f
    };
    lua::waypoint_set_settings(settings);
    
    IWaypoint::WaypointRandomizerSettings waypoint_settings;
    waypoint_settings["test"] = "Test Setting";
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, randomizer_settings).WillRepeatedly(Return(waypoint_settings));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");
    
    ASSERT_EQ(0, luaL_dostring(L, "return w.randomizer_settings"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.randomizer_settings.test"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Test Setting", lua_tostring(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return w.randomizer_settings.test2"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tonumber(L, -1));
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
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, set_route_colour(Colour(1, 2, 3)));

    lua_State* L = luaL_newstate();
    lua::colour_register(L);
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "w.colour = Colour.new(1, 2, 3)"));
}

TEST(Lua_Waypoint, SetItem)
{
    auto waypoint = mock_shared<MockWaypoint>();
    auto item = mock_shared<MockItem>();

    EXPECT_CALL(*waypoint, set_item).Times(1);

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "w.item = i"));
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
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, set_position(Vector3(1, 2, 3)));

    lua_State* L = luaL_newstate();
    lua::vector3_register(L);
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "w.position = Vector3.new(1024, 2048, 3072)"));
}

TEST(Lua_Waypoint, SetRandomizerSettings)
{
    UserSettings settings;
    settings.randomizer.settings["test"] =
    {
        .type = RandomizerSettings::Setting::Type::String,
    };
    settings.randomizer.settings["test2"] =
    {
        .type = RandomizerSettings::Setting::Type::Number,
        .default_value = 123.0f
    };
    lua::waypoint_set_settings(settings);

    IWaypoint::WaypointRandomizerSettings called_settings;
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, set_randomizer_settings).Times(AtLeast(1)).WillRepeatedly(SaveArg<0>(&called_settings));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "w.randomizer_settings = { test = \"Test string\", test2 = 150 }"));

    ASSERT_EQ("Test string", std::get<std::string>(called_settings["test"]));
    ASSERT_EQ(150.0f, std::get<float>(called_settings["test2"]));
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
    auto waypoint = mock_shared<MockWaypoint>();
    auto trigger = mock_shared<MockTrigger>();

    EXPECT_CALL(*waypoint, set_item).Times(1);

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "w.trigger = t"));
}

TEST(Lua_Waypoint, SetWaypointColour)
{
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, set_waypoint_colour(Colour(1, 2, 3)));

    lua_State* L = luaL_newstate();
    lua::colour_register(L);
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "w.waypoint_colour = Colour.new(1, 2, 3)"));
}

TEST(Lua_Waypoint, Trigger)
{
    auto trigger = mock_shared<MockTrigger>();

    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*waypoint, trigger).WillOnce(Return(trigger));

    lua_State* L = luaL_newstate();
    lua::create_waypoint(L, waypoint);
    lua_setglobal(L, "w");

    ASSERT_EQ(0, luaL_dostring(L, "return w.trigger"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
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

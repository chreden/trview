#include <trview.app/Lua/Elements/Level/Lua_Level.h>
#include <trview.app/Lua/Elements/Room/Lua_Room.h>
#include <trview.app/Lua/Elements/Trigger/Lua_Trigger.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.app/Mocks/Elements/ILight.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include "../Lua.h"

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace testing;

TEST(Lua_Level, AlternateMode)
{
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, alternate_mode).WillRepeatedly(Return(true));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.alternate_mode"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Level, SetAlternateMode)
{
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, set_alternate_mode(true));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "l.alternate_mode = true"));
}

TEST(Lua_Level, CamerasAndSinks)
{
    auto cs1 = mock_shared<MockCameraSink>()->with_number(1);
    auto cs2 = mock_shared<MockCameraSink>()->with_number(2);

    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, camera_sinks).WillRepeatedly(Return(std::vector<std::weak_ptr<ICameraSink>>{ cs1, cs2 }));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.cameras_and_sinks"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #l.cameras_and_sinks"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.cameras_and_sinks[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.cameras_and_sinks[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
}

TEST(Lua_Level, Filename)
{
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, filename).WillByDefault(testing::Return("test filename"));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l"); 

    ASSERT_EQ(0, luaL_dostring(L, "return l.filename"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("test filename", lua_tostring(L, -1));
}

TEST(Lua_Level, Floordata)
{
    std::vector<uint16_t> data { 0, 0x8004, 0x3e00, 0x0005, 0x0001, 0x0002, 0x8003 };
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, floor_data).WillRepeatedly(Return(data));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.floordata"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));

    for (std::size_t i = 0; i < data.size(); ++i)
    {
        lua_rawgeti(L, -1, i + 1);
        ASSERT_EQ(lua_tointeger(L, -1), data[i]);
        lua_pop(L, 1);
    }
}

TEST(Lua_Level, Items)
{
    auto item1 = mock_shared<MockItem>()->with_number(100);
    auto item2 = mock_shared<MockItem>()->with_number(200);
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, items).WillRepeatedly(Return(std::vector<std::weak_ptr<IItem>>{ item1, item2 }));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.items"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #l.items"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.items[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.items[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tonumber(L, -1));
}

TEST(Lua_Level, Lights)
{
    auto light1 = mock_shared<MockLight>()->with_number(100);
    auto light2 = mock_shared<MockLight>()->with_number(200);
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, lights).WillRepeatedly(Return(std::vector<std::weak_ptr<ILight>>{ light1, light2 }));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.lights"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #l.lights"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.lights[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.lights[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tonumber(L, -1));
}

TEST(Lua_Level, Rooms)
{
    auto room1 = mock_shared<MockRoom>()->with_number(100);
    auto room2 = mock_shared<MockRoom>()->with_number(200);
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, rooms).WillRepeatedly(Return(std::vector<std::weak_ptr<IRoom>>{ room1, room2 }));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.rooms"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #l.rooms"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.rooms[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.rooms[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tonumber(L, -1));
}

TEST(Lua_Level, SelectedRoom)
{
    auto room = mock_shared<MockRoom>()->with_number(200);
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, selected_room).WillRepeatedly(Return(room));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.selected_room"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.selected_room.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tonumber(L, -1));
}

TEST(Lua_Level, SetSelectedRoom)
{
    auto room = mock_shared<MockRoom>()->with_number(200);
    auto level = mock_shared<MockLevel>();
    std::weak_ptr<IRoom> called_room;
    EXPECT_CALL(*level, set_selected_room).WillOnce(SaveArg<0>(&called_room));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "l.selected_room = r"));
    ASSERT_EQ(called_room.lock(), room);
}

TEST(Lua_Level, SelectedTrigger)
{
    auto trigger = mock_shared<MockTrigger>()->with_number(200);
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, trigger(200)).WillRepeatedly(Return(trigger));
    EXPECT_CALL(*level, selected_trigger).WillRepeatedly(Return(200));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.selected_trigger"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.selected_trigger.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tonumber(L, -1));
}

TEST(Lua_Level, SetSelectedTrigger)
{
    auto trigger = mock_shared<MockTrigger>()->with_number(200);
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, set_selected_trigger(200));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "l.selected_trigger = t"));
}

TEST(Lua_Level, Triggers)
{
    auto trigger1 = mock_shared<MockTrigger>()->with_number(100);
    auto trigger2 = mock_shared<MockTrigger>()->with_number(200);
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, triggers).WillRepeatedly(Return(std::vector<std::weak_ptr<ITrigger>>{ trigger1, trigger2 }));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.triggers"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #l.triggers"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.triggers[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.triggers[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tonumber(L, -1));
}

TEST(Lua_Level, Version)
{
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, version).WillByDefault(testing::Return(trlevel::LevelVersion::Tomb4));

    LuaState L;
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.version"));
    ASSERT_EQ(4.0, lua_tonumber(L, -1));
}

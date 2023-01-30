#include <trview.app/Lua/Elements/Level/Lua_Level.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Elements/ILight.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.hpp>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace testing;

TEST(Lua_Level, CamerasAndSinks)
{
    auto level = mock_shared<MockLevel>();

    lua_State* L = luaL_newstate();
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.cameras_and_sinks"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
}

TEST(Lua_Level, Filename)
{
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, filename).WillByDefault(testing::Return("test filename"));

    lua_State* L = luaL_newstate();
    lua::create_level(L, level);
    lua_setglobal(L, "l"); 

    ASSERT_EQ(0, luaL_dostring(L, "return l.filename"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("test filename", lua_tostring(L, -1));
}

TEST(Lua_Level, Floordata)
{
    auto level = mock_shared<MockLevel>();

    lua_State* L = luaL_newstate();
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.floordata"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
}

TEST(Lua_Level, Items)
{
    auto item1 = mock_shared<MockItem>()->with_number(100);
    auto item2 = mock_shared<MockItem>()->with_number(200);
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, items).WillRepeatedly(Return(std::vector<std::weak_ptr<IItem>>{ item1, item2 }));

    lua_State* L = luaL_newstate();
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

    lua_State* L = luaL_newstate();
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

    lua_State* L = luaL_newstate();
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

TEST(Lua_Level, Triggers)
{
    auto trigger1 = mock_shared<MockTrigger>()->with_number(100);
    auto trigger2 = mock_shared<MockTrigger>()->with_number(200);
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, triggers).WillRepeatedly(Return(std::vector<std::weak_ptr<ITrigger>>{ trigger1, trigger2 }));

    lua_State* L = luaL_newstate();
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

    lua_State* L = luaL_newstate();
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.version"));
    ASSERT_EQ(4.0, lua_tonumber(L, -1));
}

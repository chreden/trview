#include <trview.app/Lua/Elements/CameraSink/Lua_CameraSink.h>
#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.hpp>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;
using namespace DirectX::SimpleMath;

TEST(Lua_CameraSink, BoxIndex)
{
    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, box_index).WillOnce(Return(123));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.box_index"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_CameraSink, Flag)
{
    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, flag).WillOnce(Return(123));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.flag"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_CameraSink, InferredRooms)
{
    auto room1 = mock_shared<MockRoom>()->with_number(100);
    auto room2 = mock_shared<MockRoom>()->with_number(200);

    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, room(100)).WillRepeatedly(Return(room1));
    EXPECT_CALL(*level, room(200)).WillRepeatedly(Return(room2));

    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, level).WillRepeatedly(Return(level));
    EXPECT_CALL(*cs, inferred_rooms).WillRepeatedly(Return(std::vector<uint16_t>{ 100, 200 }));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.inferred_rooms"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #c.inferred_rooms"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return c.inferred_rooms[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return c.inferred_rooms[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tointeger(L, -1));
}

TEST(Lua_CameraSink, Number)
{
    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, number).WillOnce(Return(123));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_CameraSink, Persistent)
{
    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, persistent).WillOnce(Return(true));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.persistent"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_CameraSink, Position)
{
    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, position).WillRepeatedly(Return(Vector3(1, 2, 3)));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.position"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return c.position.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(1024.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return c.position.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(2048.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return c.position.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(3072.0, lua_tonumber(L, -1));
}

TEST(Lua_CameraSink, Room)
{
    auto room = mock_shared<MockRoom>()->with_number(100);
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, room(100)).WillRepeatedly(Return(room));

    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, level).WillRepeatedly(Return(level));
    EXPECT_CALL(*cs, room).WillRepeatedly(Return(100));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.room"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return c.room.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tointeger(L, -1));
}

TEST(Lua_CameraSink, Strength)
{
    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, strength).WillOnce(Return(123));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.strength"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_CameraSink, Type)
{
    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, type).WillOnce(Return(ICameraSink::Type::Camera));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.type"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Camera", lua_tostring(L, -1));

    EXPECT_CALL(*cs, type).WillOnce(Return(ICameraSink::Type::Sink));
    ASSERT_EQ(0, luaL_dostring(L, "return c.type"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Sink", lua_tostring(L, -1));
}

TEST(Lua_CameraSink, TriggeredBy)
{
    auto trigger1 = mock_shared<MockTrigger>()->with_number(100);
    auto trigger2 = mock_shared<MockTrigger>()->with_number(200);

    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, triggers).WillRepeatedly(Return(std::vector<std::weak_ptr<ITrigger>>{ trigger1, trigger2 }));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.triggered_by"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #c.triggered_by"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return c.triggered_by[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return c.triggered_by[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tointeger(L, -1));
}

TEST(Lua_CameraSink, Visible)
{
    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, visible).WillOnce(Return(true));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.visible"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_CameraSink, SetType)
{
    auto level = mock_shared<MockLevel>();

    bool level_changed_raised = false;
    auto token = level->on_level_changed += [&]()
    {
        level_changed_raised = true;
    };

    auto cs = mock_shared<MockCameraSink>();
    EXPECT_CALL(*cs, visible).WillOnce(Return(true));
    EXPECT_CALL(*cs, level).WillRepeatedly(Return(level));
    EXPECT_CALL(*cs, set_type(ICameraSink::Type::Sink)).Times(1);

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.type"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Camera", lua_tostring(L, -1));

    ASSERT_EQ(0, luaL_dostring(L, "c.type = \"Sink\""));
    ASSERT_TRUE(level_changed_raised);
}

TEST(Lua_CameraSink, SetVisible)
{
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, set_camera_sink_visibility(100, true));

    auto cs = mock_shared<MockCameraSink>()->with_number(100);
    EXPECT_CALL(*cs, level).WillRepeatedly(Return(level));

    lua_State* L = luaL_newstate();
    lua::create_camera_sink(L, cs);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "c.visible = true"));
}

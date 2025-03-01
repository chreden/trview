#include <trview.app/Lua/Elements/Trigger/Lua_Trigger.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <trview.app/Lua/Colour.h>
#include "../Lua.h"

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;
using namespace DirectX::SimpleMath;

TEST(Lua_Trigger, Commands)
{
    Command command{ 123, TriggerCommandType::Camera, { 6 } };
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, commands).WillRepeatedly(Return(std::vector<Command>{ command }));

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "return t.commands"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #t.commands"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return t.commands[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return t.commands[1].type"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Camera", lua_tostring(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return t.commands[1].index"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(6, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return t.commands[1].data"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return t.commands[1].data[1]"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(6, lua_tointeger(L, -1));
}

TEST(Lua_Trigger, Flags)
{
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, flags).WillRepeatedly(Return(123));

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "return t.flags"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Trigger, Number)
{
    auto trigger = mock_shared<MockTrigger>()->with_number(123);

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "return t.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Trigger, OnlyOnce)
{
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, only_once).WillRepeatedly(Return(true));

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "return t.only_once"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Trigger, Position)
{
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, position).WillRepeatedly(Return(Vector3(1, 2, 3)));

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "return t.position"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return t.position.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(1024.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return t.position.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(2048.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return t.position.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(3072.0, lua_tonumber(L, -1));
}

TEST(Lua_Trigger, Room)
{
    auto room = mock_shared<MockRoom>()->with_number(100);
    auto trigger = mock_shared<MockTrigger>()->with_number(100);
    EXPECT_CALL(*trigger, room).WillRepeatedly(Return(room));

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "return t.room"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return t.room.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tonumber(L, -1));
}

TEST(Lua_Trigger, Sector)
{
    auto sector = mock_shared<MockSector>()->with_id(123);

    auto room = mock_shared<MockRoom>()->with_number(100);
    EXPECT_CALL(*room, sectors).WillRepeatedly(Return(std::vector<std::shared_ptr<ISector>>{ sector }));

    auto trigger = mock_shared<MockTrigger>()->with_number(100);
    EXPECT_CALL(*trigger, room).WillRepeatedly(Return(room));

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "return t.sector"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return t.sector.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tonumber(L, -1));
}

TEST(Lua_Trigger, Timer)
{
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, timer).WillRepeatedly(Return(123));

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "return t.timer"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Trigger, Type)
{
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, type).WillRepeatedly(Return(TriggerType::HeavyTrigger));

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "return t.type"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Heavy Trigger", lua_tostring(L, -1));
}

TEST(Lua_Trigger, Visible)
{
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, visible).WillRepeatedly(Return(true));

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "return t.visible"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Trigger, SetVisible)
{
    auto trigger = mock_shared<MockTrigger>()->with_number(100);
    EXPECT_CALL(*trigger, set_visible(true)).Times(1);

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "t.visible = true"));
}

TEST(Lua_Trigger, Colour)
{
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, colour).WillRepeatedly(Return(trview::Colour(1,2,3,4)));

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "x = t.colour return x"));
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

TEST(Lua_Trigger, SetColourDefault)
{
    std::optional<Colour> arg;
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, set_colour).WillOnce(SaveArg<0>(&arg));

    LuaState L;
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "t.colour = nil"));
    ASSERT_FALSE(arg);
}

TEST(Lua_Trigger, SetColour)
{
    std::optional<Colour> arg;
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(*trigger, set_colour).WillOnce(SaveArg<0>(&arg));

    LuaState L;
    lua::colour_register(L);
    lua::create_trigger(L, trigger);
    lua_setglobal(L, "t");

    ASSERT_EQ(0, luaL_dostring(L, "t.colour = Colour.new(1.0, 2.0, 3.0, 4.0)"));
    ASSERT_TRUE(arg);
    ASSERT_EQ(arg->a, 4.0f);
    ASSERT_EQ(arg->r, 1.0f);
    ASSERT_EQ(arg->g, 2.0f);
    ASSERT_EQ(arg->b, 3.0f);
}


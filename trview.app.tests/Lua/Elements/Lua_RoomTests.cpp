#include <trview.app/Lua/Elements/Room/Lua_Room.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.hpp>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;
using namespace DirectX::SimpleMath;

TEST(Lua_Room, AlternateMode)
{
    auto room = mock_shared<MockRoom>();

    lua_State* L = luaL_newstate();
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    EXPECT_CALL(*room, alternate_mode).WillOnce(Return(IRoom::AlternateMode::None));
    ASSERT_EQ(0, luaL_dostring(L, "return r.alternate_mode"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("none", lua_tostring(L, -1));

    EXPECT_CALL(*room, alternate_mode).WillOnce(Return(IRoom::AlternateMode::HasAlternate));
    ASSERT_EQ(0, luaL_dostring(L, "return r.alternate_mode"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("has_alternate", lua_tostring(L, -1));

    EXPECT_CALL(*room, alternate_mode).WillOnce(Return(IRoom::AlternateMode::IsAlternate));
    ASSERT_EQ(0, luaL_dostring(L, "return r.alternate_mode"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("is_alternate", lua_tostring(L, -1));
}

TEST(Lua_Room, Items)
{
    auto item1 = mock_shared<MockItem>()->with_number(100);
    auto item2 = mock_shared<MockItem>()->with_number(200);

    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, items).WillRepeatedly(Return(std::vector<std::weak_ptr<IItem>>{ item1, item2 }));

    lua_State* L = luaL_newstate();
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.items"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #r.items"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.items[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.items[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tointeger(L, -1));
}

TEST(Lua_Room, Lights)
{
    auto room = mock_shared<MockRoom>();

    lua_State* L = luaL_newstate();
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.lights"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #r.lights"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0, lua_tointeger(L, -1));
}

TEST(Lua_Room, Number)
{
    auto room = mock_shared<MockRoom>()->with_number(123);

    lua_State* L = luaL_newstate();
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Room, Sectors)
{
    auto sector1 = mock_shared<MockSector>()->with_id(100);
    auto sector2 = mock_shared<MockSector>()->with_id(200);

    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, sectors).WillRepeatedly(Return(std::vector<std::shared_ptr<ISector>>{ sector1, sector2 }));

    lua_State* L = luaL_newstate();
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.sectors"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #r.sectors"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.sectors[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.sectors[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tointeger(L, -1));
}

TEST(Lua_Room, Triggers)
{
    auto trigger1 = mock_shared<MockTrigger>()->with_number(100);
    auto trigger2 = mock_shared<MockTrigger>()->with_number(200);

    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, triggers).WillRepeatedly(Return(std::vector<std::weak_ptr<ITrigger>>{ trigger1, trigger2 }));

    lua_State* L = luaL_newstate();
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.triggers"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #r.triggers"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.triggers[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.triggers[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tointeger(L, -1));
}

TEST(Lua_Room, Visible)
{
    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, visible).WillOnce(Return(true));

    lua_State* L = luaL_newstate();
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.visible"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Room, SetVisible)
{
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, set_room_visibility(100, true));

    auto room = mock_shared<MockRoom>()->with_number(100);
    EXPECT_CALL(*room, level).WillRepeatedly(Return(level));

    lua_State* L = luaL_newstate();
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "r.visible = true"));
}

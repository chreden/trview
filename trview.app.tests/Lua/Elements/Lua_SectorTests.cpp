#include <trview.app/Lua/Elements/Sector/Lua_Sector.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;
using namespace DirectX::SimpleMath;

TEST(Lua_Sector, Above)
{
    auto level = mock_shared<MockLevel>();
    auto room = mock_shared<MockRoom>()->with_level(level)->with_number(10);
    EXPECT_CALL(*level, room(10)).WillRepeatedly(Return(room));

    auto sector = mock_shared<MockSector>()->with_room(room)->with_room_above(10);

    lua_State* L = luaL_newstate();
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.above"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.above.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(10, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Below)
{
    auto level = mock_shared<MockLevel>();
    auto room = mock_shared<MockRoom>()->with_level(level)->with_number(10);
    EXPECT_CALL(*level, room(10)).WillRepeatedly(Return(room));

    auto sector = mock_shared<MockSector>()->with_room(room)->with_room_below(10);

    lua_State* L = luaL_newstate();
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.below"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.below.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(10, lua_tointeger(L, -1));
}

TEST(Lua_Sector, CeilingCorners)
{
    FAIL();
}

TEST(Lua_Sector, Corners)
{
    FAIL();
}

TEST(Lua_Sector, Flags)
{
    auto sector = mock_shared<MockSector>()->with_flags(SectorFlag::Portal);

    lua_State* L = luaL_newstate();
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.flags"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
}

TEST(Lua_Sector, HasFlag)
{
    auto sector = mock_shared<MockSector>()->with_flags(SectorFlag::Portal);

    lua_State* L = luaL_newstate();
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");
    lua::sector_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return s:has_flag(Sector.Flags.Portal)"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Sector, Number)
{
    auto sector = mock_shared<MockSector>()->with_id(123);

    lua_State* L = luaL_newstate();
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Portal)
{
    auto level = mock_shared<MockLevel>();
    auto room = mock_shared<MockRoom>()->with_level(level)->with_number(10);
    ON_CALL(*level, room(10)).WillByDefault(Return(room));

    auto sector = mock_shared<MockSector>()->with_room(room)->with_portal(10);
    ON_CALL(*sector, is_portal).WillByDefault(Return(true));

    lua_State* L = luaL_newstate();
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.portal"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.portal.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(10, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Room)
{
    auto room = mock_shared<MockRoom>()->with_number(10);
    auto sector = mock_shared<MockSector>()->with_room(room);

    lua_State* L = luaL_newstate();
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.room"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.room.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(10, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Trigger)
{
    auto trigger = mock_shared<MockTrigger>()->with_number(10);
    auto sector = mock_shared<MockSector>()->with_trigger(trigger);

    lua_State* L = luaL_newstate();
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.trigger"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.trigger.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(10, lua_tointeger(L, -1));
}

TEST(Lua_Sector, X)
{
    auto sector = mock_shared<MockSector>()->with_x(123);

    lua_State* L = luaL_newstate();
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Z)
{
    auto sector = mock_shared<MockSector>()->with_z(123);

    lua_State* L = luaL_newstate();
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}
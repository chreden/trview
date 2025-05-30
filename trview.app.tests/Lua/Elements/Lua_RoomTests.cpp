#include <trview.app/Lua/Elements/Room/Lua_Room.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.app/Mocks/Elements/ILight.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include "../Lua.h"

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;
using namespace DirectX::SimpleMath;

TEST(Lua_Room, AlternateMode)
{
    auto room = mock_shared<MockRoom>();

    LuaState L;
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

TEST(Lua_Room, AlternateGroup)
{
    auto room = mock_shared<MockRoom>()->with_alternate_group(5);

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.alternate_group"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(5, lua_tointeger(L, -1));
}

TEST(Lua_Room, AlternateRoom)
{
    auto alternate = mock_shared<MockRoom>()->with_number(5);

    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, room(5)).WillByDefault(Return(alternate));

    auto room = mock_shared<MockRoom>()->with_alternate_room(5);
    ON_CALL(*room, level).WillByDefault(Return(level));

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.alternate_room"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.alternate_room.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(5, lua_tointeger(L, -1));
}

TEST(Lua_Room, CamerasAndSinks)
{
    auto cs1 = mock_shared<MockCameraSink>()->with_number(1);
    auto cs2 = mock_shared<MockCameraSink>()->with_number(2);

    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, camera_sinks).WillRepeatedly(Return(std::vector<std::weak_ptr<ICameraSink>>{ cs1, cs2 }));

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.cameras_and_sinks"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #r.cameras_and_sinks"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.cameras_and_sinks[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.cameras_and_sinks[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
}

TEST(Lua_Room, Flags)
{
    auto room = mock_shared<MockRoom>()->with_flags(static_cast<uint16_t>(IRoom::Flag::Water));

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.flags"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
}

TEST(Lua_Room, HasFlag)
{
    auto room = mock_shared<MockRoom>()->with_flags(static_cast<uint16_t>(IRoom::Flag::Water));

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");
    lua::room_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "return r:has_flag(Room.Flags.Water)"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Room, Items)
{
    auto item1 = mock_shared<MockItem>()->with_number(100);
    auto item2 = mock_shared<MockItem>()->with_number(200);

    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, items).WillRepeatedly(Return(std::vector<std::weak_ptr<IItem>>{ item1, item2 }));

    LuaState L;
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

TEST(Lua_Room, ItemsNg)
{
    auto item1 = mock_shared<MockItem>()->with_number(100);
    auto item2 = mock_shared<MockItem>()->with_number(200)->with_ng_plus(false);
    auto item3 = mock_shared<MockItem>()->with_number(201)->with_ng_plus(true);

    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, items).WillRepeatedly(Return(std::vector<std::weak_ptr<IItem>>{ item1, item2, item3 }));

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.items_ng"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #r.items_ng"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.items_ng[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.items_ng[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(201, lua_tointeger(L, -1));
}

TEST(Lua_Room, Level)
{
    auto level = mock_shared<MockLevel>()->with_version(trlevel::LevelVersion::Tomb4);
    auto room = mock_shared<MockRoom>()->with_level(level);

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.level"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.level.version"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(4, lua_tointeger(L, -1));
}

TEST(Lua_Room, Lights)
{
    auto light1 = mock_shared<MockLight>()->with_number(100);
    auto light2 = mock_shared<MockLight>()->with_number(200);

    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, lights).WillRepeatedly(Return(std::vector<std::weak_ptr<ILight>>{ light1, light2 }));

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.lights"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #r.lights"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.lights[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.lights[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tonumber(L, -1));
}

TEST(Lua_Room, Number)
{
    auto room = mock_shared<MockRoom>()->with_number(123);

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Room, NumXSectors)
{
    auto room = mock_shared<MockRoom>()->with_num_x_sectors(123);

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.num_x_sectors"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Room, NumZSectors)
{
    auto room = mock_shared<MockRoom>()->with_num_z_sectors(123);

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.num_z_sectors"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Room, Position)
{
    constexpr RoomInfo info{ .x = 1000, .z = 3000, .yBottom = 2000 };
    auto room = mock_shared<MockRoom>()->with_room_info(info);

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.position.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1000, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.position.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2000, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r.position.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(3000, lua_tonumber(L, -1));
}

TEST(Lua_Room, Sector)
{
    auto sector = mock_shared<MockSector>()->with_id(123);
    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, sector(0, 1)).WillRepeatedly(Return(sector));

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r:sector(1, 2)"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return r:sector(1, 2).number"));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Room, Sectors)
{
    auto sector1 = mock_shared<MockSector>()->with_id(100);
    auto sector2 = mock_shared<MockSector>()->with_id(200);

    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(*room, sectors).WillRepeatedly(Return(std::vector<std::shared_ptr<ISector>>{ sector1, sector2 }));

    LuaState L;
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

    LuaState L;
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

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "return r.visible"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Room, SetVisible)
{
    auto room = mock_shared<MockRoom>()->with_number(100);
    EXPECT_CALL(*room, set_visible(true)).Times(1);

    LuaState L;
    lua::create_room(L, room);
    lua_setglobal(L, "r");

    ASSERT_EQ(0, luaL_dostring(L, "r.visible = true"));
}


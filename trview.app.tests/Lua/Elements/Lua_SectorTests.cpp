#include <trview.app/Lua/Elements/Sector/Lua_Sector.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include "../Lua.h"

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

    LuaState L;
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

    LuaState L;
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
    auto room = mock_shared<MockRoom>();
    std::array<float, 4> corners
    {
        0.0f, 0.25f, 0.5f, 0.75f
    };

    auto sector = mock_shared<MockSector>()->with_room(room);
    ON_CALL(*sector, ceiling_corners).WillByDefault(Return(corners));

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.ceiling_corners"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.ceiling_corners[1]"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.ceiling_corners[2]"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.ceiling_corners[3]"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.ceiling_corners[4]"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(3, lua_tointeger(L, -1));
}

TEST(Lua_Sector, CeilingTriangulation)
{
    auto sector = mock_shared<MockSector>()->with_ceiling_triangulation(TriangulationDirection::NeSw);

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.ceiling_triangulation"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("NESW", lua_tostring(L, -1));
}

TEST(Lua_Sector, Corners)
{
    auto room = mock_shared<MockRoom>();
    std::array<float, 4> corners
    {
        0.0f, -0.25f, -0.5f, -0.75f
    };

    auto sector = mock_shared<MockSector>()->with_room(room);
    ON_CALL(*sector, corners).WillByDefault(Return(corners));

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.corners"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.corners[1]"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.corners[2]"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.corners[3]"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.corners[4]"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(3, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Flags)
{
    auto sector = mock_shared<MockSector>()->with_flags(SectorFlag::Portal);

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.flags"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Floordata)
{
    std::vector<uint16_t> data { 0, 0x8004, 0x3e00, 0x0005, 0x0001, 0x0002, 0x8003 };
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, floor_data).WillRepeatedly(Return(data));
    auto room = mock_shared<MockRoom>()->with_level(level);
    auto sector = mock_shared<MockSector>()->with_room(room);
    EXPECT_CALL(*sector, floordata_index).WillRepeatedly(Return(1));

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.floordata"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));

    for (std::size_t i = 1; i < data.size(); ++i)
    {
        lua_rawgeti(L, -1, i);
        ASSERT_EQ(lua_tointeger(L, -1), data[i]);
        lua_pop(L, 1);
    }
}

TEST(Lua_Sector, HasFlag)
{
    auto sector = mock_shared<MockSector>()->with_flags(SectorFlag::Portal);

    LuaState L;
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

    LuaState L;
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

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.portal"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.portal.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(10, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Portals)
{
    auto level = mock_shared<MockLevel>();
    auto room_10 = mock_shared<MockRoom>()->with_level(level)->with_number(10);
    ON_CALL(*level, room(10)).WillByDefault(Return(room_10));
    auto room_12 = mock_shared<MockRoom>()->with_level(level)->with_number(12);
    ON_CALL(*level, room(12)).WillByDefault(Return(room_12));

    auto sector = mock_shared<MockSector>()->with_room(room_10)->with_portals({ 10, 12 });
    ON_CALL(*sector, is_portal).WillByDefault(Return(true));

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.portals"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.portals[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(10, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.portals[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(12, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Room)
{
    auto room = mock_shared<MockRoom>()->with_number(10);
    auto sector = mock_shared<MockSector>()->with_room(room);

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.room"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.room.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(10, lua_tointeger(L, -1));
}

TEST(Lua_Sector, SectorAbove)
{
    auto other_sector = mock_shared<MockSector>()->with_id(123);
    auto other_room = mock_shared<MockRoom>();
    EXPECT_CALL(*other_room, sector).WillRepeatedly(testing::Return(other_sector));

    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, room(10)).WillRepeatedly(testing::Return(other_room));

    auto room = mock_shared<MockRoom>()->with_level(level);
    auto sector = mock_shared<MockSector>()->with_room(room)->with_room_above(10);

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.sector_above"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.sector_above.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Sector, SectorBelow)
{
    auto other_sector = mock_shared<MockSector>()->with_id(123);
    auto other_room = mock_shared<MockRoom>();
    EXPECT_CALL(*other_room, sector).WillRepeatedly(testing::Return(other_sector));

    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, room(10)).WillRepeatedly(testing::Return(other_room));

    auto room = mock_shared<MockRoom>()->with_level(level);
    auto sector = mock_shared<MockSector>()->with_room(room)->with_room_below(10);

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.sector_below"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.sector_below.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Trigger)
{
    auto trigger = mock_shared<MockTrigger>()->with_number(10);
    auto sector = mock_shared<MockSector>()->with_trigger(trigger);

    LuaState L;
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

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Z)
{
    auto sector = mock_shared<MockSector>()->with_z(123);

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Sector, Triangulation)
{
    auto sector = mock_shared<MockSector>()->with_triangulation(TriangulationDirection::NeSw);

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.triangulation"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("NESW", lua_tostring(L, -1));
}

TEST(Lua_Sector, TiltX)
{
    auto sector = mock_shared<MockSector>();
    EXPECT_CALL(*sector, tilt_x).WillRepeatedly(Return(-3));

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.tilt_x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(-3, lua_tonumber(L, -1));
}

TEST(Lua_Sector, TiltZ)
{
    auto sector = mock_shared<MockSector>();
    EXPECT_CALL(*sector, tilt_z).WillRepeatedly(Return(-3));

    LuaState L;
    lua::create_sector(L, sector);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.tilt_z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(-3, lua_tonumber(L, -1));
}

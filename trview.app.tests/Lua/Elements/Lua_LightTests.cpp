#include <trview.app/Lua/Elements/Light/Lua_Light.h>
#include <trview.app/Mocks/Elements/ILight.h>
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

TEST(Lua_Light, Colour)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, colour).WillRepeatedly(Return(Color(1, 2, 3, 4)));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.colour"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.colour.r"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(1.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.colour.g"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(2.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.colour.b"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(3.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.colour.a"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(4.0, lua_tonumber(L, -1));
}

TEST(Lua_Light, Cutoff)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, cutoff).WillOnce(Return(123.0f));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.cutoff"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tonumber(L, -1));
}

TEST(Lua_Light, Density)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, density).WillOnce(Return(123.0f));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.density"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tonumber(L, -1));
}

TEST(Lua_Light, Direction)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, direction).WillRepeatedly(Return(Vector3(1, 2, 3)));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.direction"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.direction.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(1.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.direction.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(2.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.direction.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(3.0, lua_tonumber(L, -1));
}

TEST(Lua_Light, Fade)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, fade).WillOnce(Return(123));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.fade"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tonumber(L, -1));
}

TEST(Lua_Light, Falloff)
{
    auto light = mock_shared<MockLight>();
    ON_CALL(*light, out).WillByDefault(Return(123.0f));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ON_CALL(*light, type).WillByDefault(Return(trlevel::LightType::Point));
    ASSERT_EQ(0, luaL_dostring(L, "return l.falloff"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tonumber(L, -1));

    ON_CALL(*light, type).WillByDefault(Return(trlevel::LightType::Shadow));
    ASSERT_EQ(0, luaL_dostring(L, "return l.falloff"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tonumber(L, -1));

    ON_CALL(*light, type).WillByDefault(Return(trlevel::LightType::Sun));
    ASSERT_EQ(0, luaL_dostring(L, "return l.falloff"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0, lua_tonumber(L, -1));
}

TEST(Lua_Light, FalloffAngle)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, out).WillOnce(Return(1.0f));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ON_CALL(*light, type).WillByDefault(Return(trlevel::LightType::Spot));
    ASSERT_EQ(0, luaL_dostring(L, "return l.falloff_angle"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(DirectX::XMConvertToDegrees(acosf(1.0f)), lua_tonumber(L, -1));

    ON_CALL(*light, type).WillByDefault(Return(trlevel::LightType::Point));
    ASSERT_EQ(0, luaL_dostring(L, "return l.falloff_angle"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0, lua_tonumber(L, -1));
}

TEST(Lua_Light, Hotspot)
{
    auto light = mock_shared<MockLight>();
    ON_CALL(*light, in).WillByDefault(Return(1.0f));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ON_CALL(*light, type).WillByDefault(Return(trlevel::LightType::Spot));
    ASSERT_EQ(0, luaL_dostring(L, "return l.hotspot"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(DirectX::XMConvertToDegrees(acosf(1.0f)), lua_tonumber(L, -1));

    ON_CALL(*light, type).WillByDefault(Return(trlevel::LightType::Point));
    ASSERT_EQ(0, luaL_dostring(L, "return l.hotspot"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tonumber(L, -1));

    ON_CALL(*light, type).WillByDefault(Return(trlevel::LightType::Shadow));
    ASSERT_EQ(0, luaL_dostring(L, "return l.hotspot"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(1, lua_tonumber(L, -1));

    ON_CALL(*light, type).WillByDefault(Return(trlevel::LightType::Sun));
    ASSERT_EQ(0, luaL_dostring(L, "return l.hotspot"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(0, lua_tonumber(L, -1));
}

TEST(Lua_Light, Intensity)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, intensity).WillOnce(Return(123));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.intensity"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tonumber(L, -1));
}

TEST(Lua_Light, Length)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, length).WillOnce(Return(123.0f));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.length"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tonumber(L, -1));
}

TEST(Lua_Light, Number)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, number).WillOnce(Return(123));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Light, Position)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, position).WillRepeatedly(Return(Vector3(1, 2, 3)));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.position"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.position.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(1024.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.position.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(2048.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.position.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(3072.0, lua_tonumber(L, -1));
}

TEST(Lua_Light, Radius)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, cutoff).WillOnce(Return(123.0f));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.cutoff"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tonumber(L, -1));
}

TEST(Lua_Light, RadIn)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, rad_in).WillOnce(Return(1.0f));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.rad_in"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(DirectX::XMConvertToDegrees(0.5f), lua_tonumber(L, -1));
}

TEST(Lua_Light, RadOut)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, rad_out).WillOnce(Return(1.0f));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.rad_out"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(DirectX::XMConvertToDegrees(0.5f), lua_tonumber(L, -1));
}

TEST(Lua_Light, Range)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, range).WillOnce(Return(123.0f));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.range"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tonumber(L, -1));
}

TEST(Lua_Light, Room)
{
    auto room = mock_shared<MockRoom>()->with_number(100);
    auto light = mock_shared<MockLight>()->with_number(100);
    EXPECT_CALL(*light, room).WillRepeatedly(Return(room));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.room"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return l.room.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tonumber(L, -1));
}

TEST(Lua_Light, Type)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, type).WillOnce(Return(trlevel::LightType::Point));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.type"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Point", lua_tostring(L, -1));
}

TEST(Lua_Light, Visible)
{
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(*light, visible).WillOnce(Return(true));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "return l.visible"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}


TEST(Lua_Light, SetVisible)
{
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, set_light_visibility(100, true));

    auto light = mock_shared<MockLight>()->with_number(100);
    EXPECT_CALL(*light, level).WillRepeatedly(Return(level));

    LuaState L;
    lua::create_light(L, light);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "l.visible = true"));
}
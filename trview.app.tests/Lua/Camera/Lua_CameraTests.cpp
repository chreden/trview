#include <trview.app/Lua/Camera/Lua_Camera.h>
#include <trview.app/Lua/Vector3.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include "../Lua.h"

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;
using namespace DirectX::SimpleMath;

TEST(Lua_Camera, Mode)
{
    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, mode).WillOnce(Return(ICamera::Mode::Axis));

    LuaState L;
    lua::create_camera(L, camera);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.mode"));
    ASSERT_EQ(LUA_TSTRING , lua_type(L, -1));
    ASSERT_STREQ("axis", lua_tostring(L, -1));
}

TEST(Lua_Camera, Target)
{
    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, target).WillRepeatedly(Return(Vector3(1, 2, 3)));

    LuaState L;
    lua::create_camera(L, camera);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "return c.target"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return c.target.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(1024.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return c.target.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(2048.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return c.target.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(3072.0, lua_tonumber(L, -1));
}

TEST(Lua_Camera, SetMode)
{
    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Free)).Times(1);

    LuaState L;
    lua::create_camera(L, camera);
    lua_setglobal(L, "c");

    ASSERT_EQ(0, luaL_dostring(L, "c.mode = \"free\""));
}

TEST(Lua_Camera, SetTargetVector3)
{
    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_target(Vector3(1, 2, 3))).Times(1);

    LuaState L;
    lua::create_camera(L, camera);
    lua_setglobal(L, "c");
    lua::vector3_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "c.target = Vector3.new(1024, 2048, 3072)"));
}

TEST(Lua_Camera, SetTargetPositionField)
{
    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_target(Vector3(1, 2, 3))).Times(1);

    LuaState L;
    lua::create_camera(L, camera);
    lua_setglobal(L, "c");
    lua::vector3_register(L);

    ASSERT_EQ(0, luaL_dostring(L, "c.target = { position = Vector3.new(1024, 2048, 3072) }"));
}

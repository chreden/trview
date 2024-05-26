#include <trview.app/Lua/Elements/StaticMesh/Lua_StaticMesh.h>
#include <trview.app/Mocks/Elements/IStaticMesh.h>
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

TEST(Lua_StaticMesh, Breakable)
{
    auto mesh = mock_shared<MockStaticMesh>();
    EXPECT_CALL(*mesh, breakable).WillOnce(Return(true));

    LuaState L;
    lua::create_static_mesh(L, mesh);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.breakable"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_StaticMesh, Collision)
{
    auto static_mesh = mock_shared<MockStaticMesh>();

    DirectX::BoundingBox box;
    DirectX::BoundingBox::CreateFromPoints(box,
        Vector3(-1024.0f, -2048.0f, -3072.0f),
        Vector3(1024.0f, 2048.0f, 3072.0f));
    EXPECT_CALL(*static_mesh, collision).WillRepeatedly(Return(box));

    LuaState L;
    lua::create_static_mesh(L, static_mesh);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.collision"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.collision.min.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(-1024.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.collision.min.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(-2048.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.collision.min.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(-3072.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.collision.max.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(1024.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.collision.max.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(2048.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.collision.max.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(3072.0, lua_tonumber(L, -1));
}

TEST(Lua_StaticMesh, Id)
{
    auto static_mesh = mock_shared<MockStaticMesh>();
    EXPECT_CALL(*static_mesh, id).WillRepeatedly(Return(123));

    LuaState L;
    lua::create_static_mesh(L, static_mesh);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.id"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_StaticMesh, Position)
{
    auto static_mesh = mock_shared<MockStaticMesh>();
    EXPECT_CALL(*static_mesh, position).WillRepeatedly(Return(Vector3(1, 2, 3)));

    LuaState L;
    lua::create_static_mesh(L, static_mesh);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.position"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.position.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(1024.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.position.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(2048.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.position.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(3072.0, lua_tonumber(L, -1));
}

TEST(Lua_StaticMesh, Room)
{
    auto room = mock_shared<MockRoom>()->with_number(100);
    auto static_mesh = mock_shared<MockStaticMesh>();
    EXPECT_CALL(*static_mesh, room).WillRepeatedly(Return(room));

    LuaState L;
    lua::create_static_mesh(L, static_mesh);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.room"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.room.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tonumber(L, -1));
}

TEST(Lua_StaticMesh, Rotation)
{
    auto static_mesh = mock_shared<MockStaticMesh>();
    EXPECT_CALL(*static_mesh, rotation).WillRepeatedly(Return(123.0f));

    LuaState L;
    lua::create_static_mesh(L, static_mesh);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.rotation"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123.0f, lua_tonumber(L, -1));
}

TEST(Lua_StaticMesh, SetVisible)
{
    auto static_mesh = mock_shared<MockStaticMesh>()->with_number(100);
    EXPECT_CALL(*static_mesh, set_visible(true)).Times(1);

    LuaState L;
    lua::create_static_mesh(L, static_mesh);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "s.visible = true"));
}

TEST(Lua_StaticMesh, Type)
{
    auto static_mesh = mock_shared<MockStaticMesh>();
    ON_CALL(*static_mesh, type).WillByDefault(Return(IStaticMesh::Type::Mesh));

    LuaState L;
    lua::create_static_mesh(L, static_mesh);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.type"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Mesh", lua_tostring(L, -1));

    ON_CALL(*static_mesh, type).WillByDefault(Return(IStaticMesh::Type::Sprite));
    ASSERT_EQ(0, luaL_dostring(L, "return s.type"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Sprite", lua_tostring(L, -1));
}

TEST(Lua_StaticMesh, Visible)
{
    auto static_mesh = mock_shared<MockStaticMesh>();
    EXPECT_CALL(*static_mesh, visible).WillOnce(Return(true));

    LuaState L;
    lua::create_static_mesh(L, static_mesh);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.visible"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_StaticMesh, Visibility)
{
    auto static_mesh = mock_shared<MockStaticMesh>();

    DirectX::BoundingBox box;
    DirectX::BoundingBox::CreateFromPoints(box,
        Vector3(-1024.0f, -2048.0f, -3072.0f),
        Vector3(1024.0f, 2048.0f, 3072.0f));
    EXPECT_CALL(*static_mesh, visibility).WillRepeatedly(Return(box));

    LuaState L;
    lua::create_static_mesh(L, static_mesh);
    lua_setglobal(L, "s");

    ASSERT_EQ(0, luaL_dostring(L, "return s.visibility"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.visibility.min.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(-1024.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.visibility.min.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(-2048.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.visibility.min.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(-3072.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.visibility.max.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(1024.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.visibility.max.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(2048.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return s.visibility.max.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(3072.0, lua_tonumber(L, -1));
}


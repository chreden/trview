#include <trview.app/Lua/Elements/Item/Lua_Item.h>
#include <trview.app/Mocks/Elements/IItem.h>
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

TEST(Lua_Item, ActivationFlags)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, activation_flags).WillOnce(Return(123));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.activation_flags"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Item, Ai)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, is_ai).WillOnce(Return(true));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.ai"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Item, Angle)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, angle).WillOnce(Return(123));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.angle"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Item, Categories)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, categories).WillRepeatedly(Return<std::unordered_set<std::string>>({ "One", "Two" }));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.categories"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return i.categories[1]"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("One", lua_tostring(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return i.categories[2]"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Two", lua_tostring(L, -1));
}

TEST(Lua_Item, ClearBody)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, clear_body_flag).WillOnce(Return(true));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.clear_body"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Item, Invisible)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, invisible_flag).WillOnce(Return(true));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.invisible"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Item, Ng)
{
    auto item = mock_shared<MockItem>()->with_ng_plus(true);

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.ng"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));

    item->with_ng_plus(false);
    ASSERT_EQ(0, luaL_dostring(L, "return i.ng"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(false, lua_toboolean(L, -1));

    item->with_ng_plus(std::nullopt);
    ASSERT_EQ(0, luaL_dostring(L, "return i.ng"));
    ASSERT_EQ(LUA_TNIL, lua_type(L, -1));
}

TEST(Lua_Item, Number)
{
    auto item = mock_shared<MockItem>()->with_number(123);

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Item, Ocb)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, ocb).WillOnce(Return(123));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.ocb"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Item, Position)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, position).WillRepeatedly(Return(Vector3(1, 2, 3)));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.position"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return i.position.x"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(1024.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return i.position.y"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(2048.0, lua_tonumber(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return i.position.z"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_DOUBLE_EQ(3072.0, lua_tonumber(L, -1));
}

TEST(Lua_Item, RemasteredExtra)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, is_remastered_extra).WillOnce(Return(true));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.remastered_extra"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Item, Room)
{
    auto room = mock_shared<MockRoom>()->with_number(100);
    auto item = mock_shared<MockItem>()->with_number(100);
    EXPECT_CALL(*item, room).WillRepeatedly(Return(room));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.room"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return i.room.number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tonumber(L, -1));
}

TEST(Lua_Item, TriggeredBy)
{
    auto trigger1 = mock_shared<MockTrigger>()->with_number(100);
    auto trigger2 = mock_shared<MockTrigger>()->with_number(200);

    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, triggers).WillRepeatedly(Return(std::vector<std::weak_ptr<ITrigger>>{ trigger1, trigger2 }));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.trigger_references"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return #i.trigger_references"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(2, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return i.trigger_references[1].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(100, lua_tointeger(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return i.trigger_references[2].number"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(200, lua_tointeger(L, -1));
}

TEST(Lua_Item, Type)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, type).WillOnce(Return("Lara"));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.type"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("Lara", lua_tostring(L, -1));
}

TEST(Lua_Item, TypeId)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, type_id).WillOnce(Return(123));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.type_id"));
    ASSERT_EQ(LUA_TNUMBER, lua_type(L, -1));
    ASSERT_EQ(123, lua_tointeger(L, -1));
}

TEST(Lua_Item, Visible)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, visible).WillOnce(Return(true));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "return i.visible"));
    ASSERT_EQ(LUA_TBOOLEAN, lua_type(L, -1));
    ASSERT_EQ(true, lua_toboolean(L, -1));
}

TEST(Lua_Item, SetCategories)
{
    std::unordered_set<std::string> categories;
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, set_categories).WillOnce(SaveArg<0>(&categories));

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "i.categories = { \"One\", \"Two\" }"));

    const std::unordered_set<std::string> expected{ "One", "Two" };
    ASSERT_EQ(categories, expected);
}
TEST(Lua_Item, SetVisible)
{
    auto item = mock_shared<MockItem>()->with_number(100);
    EXPECT_CALL(*item, set_visible(true)).Times(1);

    LuaState L;
    lua::create_item(L, item);
    lua_setglobal(L, "i");

    ASSERT_EQ(0, luaL_dostring(L, "i.visible = true"));
}
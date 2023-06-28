#include <trview.app/Lua/trview/trview.h>
#include <trview.app/Lua/Elements/Level/Lua_Level.h>
#include <trview.app/Mocks/IApplication.h>
#include <trview.tests.common/Mocks.h>
#include <external/lua/src/lua.h>
#include <external/lua/src/lauxlib.h>
#include <trview.app/Mocks/Routing/IRoute.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace testing;

TEST(Lua_trview, Level)
{
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, filename).WillByDefault(Return("test.tr2"));

    auto application = mock_shared<MockApplication>();
    EXPECT_CALL(*application, current_level).WillRepeatedly(Return(level));

    lua_State* L = luaL_newstate();
    lua::trview_register(L, application.get(), 
        []() { return mock_shared<MockRoute>(); },
        [](auto&&...) { return mock_shared<MockWaypoint>(); },
        mock_shared<MockDialogs>(),
        mock_shared<MockFiles>());

    ASSERT_EQ(0, luaL_dostring(L, "return trview.level"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return trview.level.filename"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("test.tr2", lua_tostring(L, -1));
}

TEST(Lua_trview, RecentFiles)
{
    UserSettings settings{};
    settings.recent_files = { "test1.tr2", "test2.tr2" };

    auto application = mock_shared<MockApplication>();
    EXPECT_CALL(*application, settings).WillRepeatedly(Return(settings));

    lua_State* L = luaL_newstate();
    lua::trview_register(L, application.get(),
        []() { return mock_shared<MockRoute>(); },
        [](auto&&...) { return mock_shared<MockWaypoint>(); },
        mock_shared<MockDialogs>(),
        mock_shared<MockFiles>());

    ASSERT_EQ(0, luaL_dostring(L, "return trview.recent_files"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return trview.recent_files[1]"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("test1.tr2", lua_tostring(L, -1));
    ASSERT_EQ(0, luaL_dostring(L, "return trview.recent_files[2]"));
    ASSERT_EQ(LUA_TSTRING, lua_type(L, -1));
    ASSERT_STREQ("test2.tr2", lua_tostring(L, -1));
}

TEST(Lua_trview, SetLevel)
{
    auto application = mock_shared<MockApplication>();
    EXPECT_CALL(*application, set_current_level).Times(1);

    lua_State* L = luaL_newstate();
    lua::trview_register(L, application.get(),
        []() { return mock_shared<MockRoute>(); },
        [](auto&&...) { return mock_shared<MockWaypoint>(); },
        mock_shared<MockDialogs>(),
        mock_shared<MockFiles>());

    auto level = mock_shared<MockLevel>();
    lua::create_level(L, level);
    lua_setglobal(L, "l");

    ASSERT_EQ(0, luaL_dostring(L, "trview.level = l"));
}

TEST(Lua_trview, Route)
{
    auto application = mock_shared<MockApplication>();
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*application, route).Times(1).WillRepeatedly(Return(route));

    lua_State* L = luaL_newstate();
    lua::trview_register(L, application.get(),
        []() { return mock_shared<MockRoute>(); },
        [](auto&&...) { return mock_shared<MockWaypoint>(); },
        mock_shared<MockDialogs>(),
        mock_shared<MockFiles>());

    ASSERT_EQ(0, luaL_dostring(L, "return trview.route"));
    ASSERT_EQ(LUA_TUSERDATA, lua_type(L, -1));
}

TEST(Lua_trview, SetRoute)
{
    auto application = mock_shared<MockApplication>();
    EXPECT_CALL(*application, set_route).Times(1);

    lua_State* L = luaL_newstate();
    lua::trview_register(L, application.get(),
        []() { return mock_shared<MockRoute>(); },
        [](auto&&...) { return mock_shared<MockWaypoint>(); },
        mock_shared<MockDialogs>(),
        mock_shared<MockFiles>());

    ASSERT_EQ(0, luaL_dostring(L, "trview.route = Route.new()"));
}

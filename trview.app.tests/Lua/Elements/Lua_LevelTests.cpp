#include <trview.app/Lua/Elements/Level/Lua_Level.h>
#include <trview.app/Application.h>
#include <trview.app/Mocks/Menus/IUpdateChecker.h>
#include <trview.app/Mocks/Menus/IFileMenu.h>
#include <trview.app/Mocks/Windows/ILightsWindowManager.h>
#include <trview.app/Mocks/Windows/ILogWindowManager.h>
#include <trview.app/Mocks/Windows/ITexturesWindowManager.h>
#include <trview.app/Mocks/Windows/ICameraSinkWindowManager.h>
#include "NullImGuiBackend.h"

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"ApplicationLuaTests") };
            std::unique_ptr<IUpdateChecker> update_checker{ mock_unique<MockUpdateChecker>() };
            std::unique_ptr<ISettingsLoader> settings_loader{ mock_unique<MockSettingsLoader>() };
            trlevel::ILevel::Source trlevel_source{ [](auto&&...) { return mock_unique<trlevel::mocks::MockLevel>(); } };
            std::unique_ptr<IFileMenu> file_menu{ mock_unique<MockFileMenu>() };
            std::unique_ptr<IViewer> viewer{ mock_unique<MockViewer>() };
            IRoute::Source route_source{ [](auto&&...) { return mock_shared<MockRoute>(); } };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            std::unique_ptr<IItemsWindowManager> items_window_manager{ mock_unique<MockItemsWindowManager>() };
            std::unique_ptr<ITriggersWindowManager> triggers_window_manager{ mock_unique<MockTriggersWindowManager>() };
            std::unique_ptr<IRouteWindowManager> route_window_manager{ mock_unique<MockRouteWindowManager>() };
            std::unique_ptr<IRoomsWindowManager> rooms_window_manager{ mock_unique<MockRoomsWindowManager>() };
            ILevel::Source level_source{ [](auto&&...) { return mock_unique<trview::mocks::MockLevel>(); } };
            std::shared_ptr<IStartupOptions> startup_options{ mock_shared<MockStartupOptions>() };
            std::shared_ptr<IDialogs> dialogs{ mock_shared<MockDialogs>() };
            std::shared_ptr<IFiles> files{ mock_shared<MockFiles>() };
            std::unique_ptr<IImGuiBackend> imgui_backend{ std::make_unique<NullImGuiBackend>() };
            std::unique_ptr<ILightsWindowManager> lights_window_manager{ mock_unique<MockLightsWindowManager>() };
            std::unique_ptr<ILogWindowManager> log_window_manager{ mock_unique<MockLogWindowManager>() };
            std::unique_ptr<ITexturesWindowManager> textures_window_manager{ mock_unique<MockTexturesWindowManager>() };
            std::unique_ptr<ICameraSinkWindowManager> camera_sink_window_manager{ mock_unique<MockCameraSinkWindowManager>() };

            std::unique_ptr<Application> build()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
                return std::make_unique<Application>(window, std::move(update_checker), std::move(settings_loader),
                    trlevel_source, std::move(file_menu), std::move(viewer), route_source, shortcuts,
                    std::move(items_window_manager), std::move(triggers_window_manager), std::move(route_window_manager), std::move(rooms_window_manager),
                    level_source, startup_options, dialogs, files, std::move(imgui_backend), std::move(lights_window_manager), std::move(log_window_manager),
                    std::move(textures_window_manager), std::move(camera_sink_window_manager));
            }

            test_module& with_dialogs(std::shared_ptr<IDialogs> dialogs)
            {
                this->dialogs = dialogs;
                return *this;
            }

            test_module& with_items_window_manager(std::unique_ptr<IItemsWindowManager> items_window_manager)
            {
                this->items_window_manager = std::move(items_window_manager);
                return *this;
            }

            test_module& with_trlevel_source(trlevel::ILevel::Source trlevel_source)
            {
                this->trlevel_source = trlevel_source;
                return *this;
            }

            test_module& with_file_menu(std::unique_ptr<IFileMenu> file_menu)
            {
                this->file_menu = std::move(file_menu);
                return *this;
            }

            test_module& with_rooms_window_manager(std::unique_ptr<IRoomsWindowManager> rooms_window_manager)
            {
                this->rooms_window_manager = std::move(rooms_window_manager);
                return *this;
            }

            test_module& with_route_source(IRoute::Source route_source)
            {
                this->route_source = route_source;
                return *this;
            }

            test_module& with_route_window_manager(std::unique_ptr<IRouteWindowManager> route_window_manager)
            {
                this->route_window_manager = std::move(route_window_manager);
                return *this;
            }

            test_module& with_settings_loader(std::unique_ptr<ISettingsLoader> settings_loader)
            {
                this->settings_loader = std::move(settings_loader);
                return *this;
            }

            test_module& with_startup_options(std::shared_ptr<IStartupOptions> startup_options)
            {
                this->startup_options = startup_options;
                return *this;
            }

            test_module& with_triggers_window_manager(std::unique_ptr<ITriggersWindowManager> triggers_window_manager)
            {
                this->triggers_window_manager = std::move(triggers_window_manager);
                return *this;
            }

            test_module& with_lights_window_manager(std::unique_ptr<ILightsWindowManager> lights_window_manager)
            {
                this->lights_window_manager = std::move(lights_window_manager);
                return *this;
            }

            test_module& with_update_checker(std::unique_ptr<IUpdateChecker> update_checker)
            {
                this->update_checker = std::move(update_checker);
                return *this;
            }

            test_module& with_viewer(std::unique_ptr<IViewer> viewer)
            {
                this->viewer = std::move(viewer);
                return *this;
            }

            test_module& with_files(const std::shared_ptr<IFiles>& files)
            {
                this->files = files;
                return *this;
            }

            test_module& with_imgui_backend(std::unique_ptr<IImGuiBackend> backend)
            {
                this->imgui_backend = std::move(backend);
                return *this;
            }

            test_module& with_level_source(ILevel::Source level_source)
            {
                this->level_source = level_source;
                return *this;
            }

            test_module& with_log_window_manager(std::unique_ptr<ILogWindowManager> log_window_manager)
            {
                this->log_window_manager = std::move(log_window_manager);
                return *this;
            }

            test_module& with_textures_window_manager(std::unique_ptr<ITexturesWindowManager> textures_window_manager)
            {
                this->textures_window_manager = std::move(textures_window_manager);
                return *this;
            }

            test_module& with_camera_sink_window_manager(std::unique_ptr<ICameraSinkWindowManager> camera_sink_window_manager)
            {
                this->camera_sink_window_manager = std::move(camera_sink_window_manager);
                return *this;
            }
        };
        return test_module{};
    }
}

TEST(Lua_Level, CamerasAndSinks)
{
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    ILevel::Source level_source = [&](auto&&...) { return std::move(level_ptr); };
    ON_CALL(level, version).WillByDefault(testing::Return(trlevel::LevelVersion::Tomb4));

    auto app = register_test_module()
        .with_level_source(level_source)
        .build();
    app->open("test", ILevel::OpenMode::Full);

    lua_State* L = lua_get_state();
    ASSERT_EQ(0, luaL_dostring(L, "return trview.level.cameras_and_sinks"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
}

TEST(Lua_Level, Filename)
{
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    ILevel::Source level_source = [&](auto&&...) { return std::move(level_ptr); };
    ON_CALL(level, filename).WillByDefault(testing::Return("test filename"));

    auto app = register_test_module()
        .with_level_source(level_source)
        .build();
    app->open("test", ILevel::OpenMode::Full);

    lua_State* L = lua_get_state();
    ASSERT_EQ(0, luaL_dostring(L, "return trview.level.filename"));

    std::string result = lua_tostring(L, -1);
    ASSERT_EQ(result, "test filename");
}

TEST(Lua_Level, Floordata)
{
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    ILevel::Source level_source = [&](auto&&...) { return std::move(level_ptr); };
    ON_CALL(level, version).WillByDefault(testing::Return(trlevel::LevelVersion::Tomb4));

    auto app = register_test_module()
        .with_level_source(level_source)
        .build();
    app->open("test", ILevel::OpenMode::Full);

    lua_State* L = lua_get_state();
    ASSERT_EQ(0, luaL_dostring(L, "return trview.level.floordata"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
}

TEST(Lua_Level, Items)
{
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    ILevel::Source level_source = [&](auto&&...) { return std::move(level_ptr); };
    ON_CALL(level, version).WillByDefault(testing::Return(trlevel::LevelVersion::Tomb4));

    auto app = register_test_module()
        .with_level_source(level_source)
        .build();
    app->open("test", ILevel::OpenMode::Full);

    lua_State* L = lua_get_state();
    ASSERT_EQ(0, luaL_dostring(L, "return trview.level.items"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
}

TEST(Lua_Level, Lights)
{
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    ILevel::Source level_source = [&](auto&&...) { return std::move(level_ptr); };
    ON_CALL(level, version).WillByDefault(testing::Return(trlevel::LevelVersion::Tomb4));

    auto app = register_test_module()
        .with_level_source(level_source)
        .build();
    app->open("test", ILevel::OpenMode::Full);

    lua_State* L = lua_get_state();
    ASSERT_EQ(0, luaL_dostring(L, "return trview.level.lights"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
}

TEST(Lua_Level, Rooms)
{
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    ILevel::Source level_source = [&](auto&&...) { return std::move(level_ptr); };
    ON_CALL(level, version).WillByDefault(testing::Return(trlevel::LevelVersion::Tomb4));

    auto app = register_test_module()
        .with_level_source(level_source)
        .build();
    app->open("test", ILevel::OpenMode::Full);

    lua_State* L = lua_get_state();
    ASSERT_EQ(0, luaL_dostring(L, "return trview.level.rooms"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
}

TEST(Lua_Level, Triggers)
{
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    ILevel::Source level_source = [&](auto&&...) { return std::move(level_ptr); };
    ON_CALL(level, version).WillByDefault(testing::Return(trlevel::LevelVersion::Tomb4));

    auto app = register_test_module()
        .with_level_source(level_source)
        .build();
    app->open("test", ILevel::OpenMode::Full);

    lua_State* L = lua_get_state();
    ASSERT_EQ(0, luaL_dostring(L, "return trview.level.triggers"));
    ASSERT_EQ(LUA_TTABLE, lua_type(L, -1));
}

TEST(Lua_Level, Version)
{
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    ILevel::Source level_source = [&](auto&&...) { return std::move(level_ptr); };
    ON_CALL(level, version).WillByDefault(testing::Return(trlevel::LevelVersion::Tomb4));

    auto app = register_test_module()
        .with_level_source(level_source)
        .build();
    app->open("test", ILevel::OpenMode::Full);

    lua_State* L = lua_get_state();
    ASSERT_EQ(0, luaL_dostring(L, "return trview.level.version"));

    double result = lua_tonumber(L, -1);
    ASSERT_EQ(result, 4.0);
}

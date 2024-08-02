#include <trview.app/Application.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Elements/ILight.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.app/Mocks/Menus/IUpdateChecker.h>
#include <trview.app/Mocks/Menus/IFileMenu.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trview.app/Mocks/Routing/IRandomizerRoute.h>
#include <trview.app/Mocks/Settings/ISettingsLoader.h>
#include <trview.app/Mocks/Settings/IStartupOptions.h>
#include <trview.app/Mocks/UI/IImGuiBackend.h>
#include <trview.app/Mocks/Windows/IViewer.h>
#include <trview.app/Mocks/Windows/IItemsWindowManager.h>
#include <trview.app/Mocks/Windows/ITriggersWindowManager.h>
#include <trview.app/Mocks/Windows/IRouteWindowManager.h>
#include <trview.app/Mocks/Windows/IRoomsWindowManager.h>
#include <trview.app/Mocks/Windows/ILightsWindowManager.h>
#include <trview.app/Mocks/Windows/ILogWindowManager.h>
#include <trview.app/Mocks/Windows/ITexturesWindowManager.h>
#include <trview.app/Mocks/Windows/ICameraSinkWindowManager.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.common/Mocks/IFiles.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Resources/resource.h>
#include "NullImGuiBackend.h"
#include <trview.common/Strings.h>
#include <ranges>
#include <trview.app/Mocks/Windows/IConsoleManager.h>
#include <trview.app/Mocks/Lua/ILua.h>
#include <trview.app/Mocks/Plugins/IPlugins.h>
#include <trview.app/Mocks/Windows/IPluginsWindowManager.h>
#include <trview.app/Mocks/UI/IFonts.h>
#include <trview.app/Mocks/Windows/IWindows.h>

using namespace trview;
using namespace trview::tests;
using namespace testing;
using namespace trview::mocks;
using namespace trlevel::mocks;
using testing::_;

namespace
{
    std::string fonts_directory()
    {
        wchar_t* path;
        if (S_OK != SHGetKnownFolderPath(FOLDERID_Fonts, 0, nullptr, &path))
        {
            return std::string();
        }
        auto result = trview::to_utf8(path);
        CoTaskMemFree(path);
        return result;
    }

    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"ApplicationTests") };
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
            std::shared_ptr<IImGuiBackend> imgui_backend{ std::make_shared<NullImGuiBackend>() };
            std::unique_ptr<ILightsWindowManager> lights_window_manager{ mock_unique<MockLightsWindowManager>() };
            std::unique_ptr<ILogWindowManager> log_window_manager{ mock_unique<MockLogWindowManager>() };
            std::unique_ptr<ITexturesWindowManager> textures_window_manager{ mock_unique<MockTexturesWindowManager>() };
            std::unique_ptr<ICameraSinkWindowManager> camera_sink_window_manager{ mock_unique<MockCameraSinkWindowManager>() };
            std::unique_ptr<IConsoleManager> console_manager{ mock_unique<MockConsoleManager>() };
            std::shared_ptr<IPlugins> plugins{ mock_shared<MockPlugins>() };
            std::unique_ptr<IPluginsWindowManager> plugins_window_manager{ mock_unique<MockPluginsWindowManager>() };
            IRandomizerRoute::Source randomizer_route_source { [](auto&&...) { return mock_shared<MockRandomizerRoute>(); } };
            std::shared_ptr<IFonts> fonts { mock_shared<MockFonts>() };
            std::unique_ptr<IWindows> windows{ mock_unique<MockWindows>() };

            std::unique_ptr<Application> build()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
                return std::make_unique<Application>(window, std::move(update_checker), std::move(settings_loader),
                    trlevel_source, std::move(file_menu), std::move(viewer), route_source, shortcuts,
                    std::move(items_window_manager), std::move(triggers_window_manager), std::move(route_window_manager), std::move(rooms_window_manager),
                    level_source, startup_options, dialogs, files, std::move(imgui_backend), std::move(lights_window_manager), std::move(log_window_manager),
                    std::move(textures_window_manager), std::move(camera_sink_window_manager), std::move(console_manager),
                    plugins, std::move(plugins_window_manager), randomizer_route_source, fonts, std::move(windows),
                    Application::LoadMode::Sync);
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

            test_module& with_randomizer_route_source(IRandomizerRoute::Source randomizer_route_source)
            {
                this->randomizer_route_source = randomizer_route_source;
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

            test_module& with_imgui_backend(std::shared_ptr<IImGuiBackend> backend)
            {
                this->imgui_backend = backend;
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

            test_module& with_console_manager(std::unique_ptr<IConsoleManager> console_manager)
            {
                this->console_manager = std::move(console_manager);
                return *this;
            }

            test_module& with_plugins(std::shared_ptr<IPlugins> plugins)
            {
                this->plugins = plugins;
                return *this;
            }

            test_module& with_plugins_window_manager(std::unique_ptr<IPluginsWindowManager> plugins_window_manager)
            {
                this->plugins_window_manager = std::move(plugins_window_manager);
                return *this;
            }

            test_module& with_windows(std::unique_ptr<IWindows> windows)
            {
                this->windows = std::move(windows);
                return *this;
            }

            test_module& with_fonts(std::shared_ptr<IFonts> fonts)
            {
                this->fonts = fonts;
                return *this;
            }
        };
        return test_module{};
    }
}

TEST(Application, ChecksForUpdates)
{
    auto [update_checker_ptr, update_checker] = create_mock<MockUpdateChecker>();
    EXPECT_CALL(update_checker, check_for_updates).Times(1);
    auto application = register_test_module().with_update_checker(std::move(update_checker_ptr)).build();
}

TEST(Application, SettingsLoadedAndSaved)
{
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    EXPECT_CALL(settings_loader, load_user_settings).Times(1);
    EXPECT_CALL(settings_loader, save_user_settings).Times(1);
    auto application = register_test_module().with_settings_loader(std::move(settings_loader_ptr)).build();
}

TEST(Application, LevelLoadedOnFileOpen)
{
    auto [file_menu_ptr, file_menu] = create_mock<MockFileMenu>();
    std::optional<std::string> called;
    auto trlevel_source = [&](auto&& filename)-> std::unique_ptr<trlevel::ILevel> { called = filename; throw std::exception(); };
    auto application = register_test_module().with_trlevel_source(trlevel_source).with_file_menu(std::move(file_menu_ptr)).build();
    file_menu.on_file_open("test_path.tr2");
    ASSERT_TRUE(called.has_value());
    ASSERT_EQ(called.value(), "test_path.tr2");
}

TEST(Application, RecentFilesUpdatedOnFileOpen)
{
    auto [file_menu_ptr, file_menu] = create_mock<MockFileMenu>();
    EXPECT_CALL(file_menu, set_recent_files(std::list<std::string>{})).Times(1);
    EXPECT_CALL(file_menu, set_recent_files(std::list<std::string>{"test_path.tr2"})).Times(1);
    std::optional<std::string> called;
    auto trlevel_source = [&](auto&& filename) { called = filename; return mock_unique<trlevel::mocks::MockLevel>(); };
    auto application = register_test_module().with_trlevel_source(trlevel_source).with_file_menu(std::move(file_menu_ptr)).build();
    application->open("test_path.tr2", ILevel::OpenMode::Full);
    ASSERT_TRUE(called.has_value());
    ASSERT_EQ(called.value(), "test_path.tr2");
}

TEST(Application, FileOpenedInViewer)
{
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    std::optional<std::string> called;
    auto trlevel_source = [&](auto&& filename) { called = filename; return mock_unique<trlevel::mocks::MockLevel>(); };
    EXPECT_CALL(viewer, open(A<const std::weak_ptr<ILevel>&>(), ILevel::OpenMode::Full)).Times(1);
    auto application = register_test_module().with_trlevel_source(trlevel_source).with_viewer(std::move(viewer_ptr)).build();
    application->open("test_path.tr2", ILevel::OpenMode::Full);
    ASSERT_TRUE(called.has_value());
    ASSERT_EQ(called.value(), "test_path.tr2");
}

TEST(Application, WindowContentsResetBeforeViewerLoaded)
{
    std::optional<std::string> called;
    auto trlevel_source = [&](auto&& filename) { called = filename; return mock_unique<trlevel::mocks::MockLevel>(); };
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    auto [items_window_manager_ptr, items_window_manager] = create_mock<MockItemsWindowManager>();
    auto [rooms_window_manager_ptr, rooms_window_manager] = create_mock<MockRoomsWindowManager>();
    auto [triggers_window_manager_ptr, triggers_window_manager] = create_mock<MockTriggersWindowManager>();
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto [lights_window_manager_ptr, lights_window_manager] = create_mock<MockLightsWindowManager>();
    auto [textures_window_manager_ptr, textures_window_manager] = create_mock<MockTexturesWindowManager>();
    auto [camera_sink_window_manager_ptr, camera_sink_window_manager] = create_mock<MockCameraSinkWindowManager>();
    auto [windows_ptr, windows] = create_mock<MockWindows>();
    auto route = mock_shared<MockRoute>();

    std::vector<std::string> events;

    EXPECT_CALL(items_window_manager, set_items(A<const std::vector<std::weak_ptr<IItem>>&>())).Times(1).WillOnce([&](auto) { events.push_back("items_items"); });
    EXPECT_CALL(items_window_manager, set_triggers(A<const std::vector<std::weak_ptr<ITrigger>>&>())).Times(1).WillOnce([&](auto) { events.push_back("items_triggers"); });
    EXPECT_CALL(items_window_manager, set_level_version(A<trlevel::LevelVersion>())).Times(1).WillOnce([&](auto) { events.push_back("items_version"); });
    EXPECT_CALL(items_window_manager, set_model_checker(A<const std::function<bool (uint32_t)>&>())).Times(1).WillOnce([&](auto) { events.push_back("items_model_checker"); });
    EXPECT_CALL(triggers_window_manager, set_items(A<const std::vector<std::weak_ptr<IItem>>&>())).Times(1).WillOnce([&](auto) { events.push_back("triggers_items"); });
    EXPECT_CALL(triggers_window_manager, set_triggers(A<const std::vector<std::weak_ptr<ITrigger>>&>())).Times(1).WillOnce([&](auto) { events.push_back("triggers_triggers"); });
    EXPECT_CALL(rooms_window_manager, set_level_version(A<trlevel::LevelVersion>())).Times(1).WillOnce([&](auto) { events.push_back("rooms_version"); });
    EXPECT_CALL(rooms_window_manager, set_items(A<const std::vector<std::weak_ptr<IItem>>&>())).Times(1).WillOnce([&](auto) { events.push_back("rooms_items"); });
    EXPECT_CALL(rooms_window_manager, set_floordata(A<const std::vector<uint16_t>&>())).Times(1).WillOnce([&](auto) { events.push_back("rooms_floordata"); });
    EXPECT_CALL(rooms_window_manager, set_rooms(A<const std::vector<std::weak_ptr<IRoom>>&>())).Times(1).WillOnce([&](auto) { events.push_back("rooms_rooms"); });
    EXPECT_CALL(route_window_manager, set_items(A<const std::vector<std::weak_ptr<IItem>>&>())).Times(1).WillOnce([&](auto) { events.push_back("route_items"); });
    EXPECT_CALL(route_window_manager, set_triggers(A<const std::vector<std::weak_ptr<ITrigger>>&>())).Times(1).WillOnce([&](auto) { events.push_back("route_triggers"); });
    EXPECT_CALL(route_window_manager, set_rooms(A<const std::vector<std::weak_ptr<IRoom>>&>())).Times(1).WillOnce([&](auto) { events.push_back("route_rooms"); });
    EXPECT_CALL(route_window_manager, set_route(A<const std::weak_ptr<IRoute>&>())).Times(3).WillRepeatedly([&](auto) { events.push_back("route_route"); });
    EXPECT_CALL(lights_window_manager, set_lights(A<const std::vector<std::weak_ptr<ILight>>&>())).Times(1).WillOnce([&](auto) { events.push_back("lights_lights"); });
    EXPECT_CALL(camera_sink_window_manager, set_camera_sinks).Times(1).WillOnce([&](auto) { events.push_back("camera_sinks_camera_sinks"); });
    EXPECT_CALL(windows, set_level).Times(1).WillOnce([&](auto) { events.push_back("windows_level"); });
    EXPECT_CALL(*route, clear()).Times(1).WillOnce([&] { events.push_back("route_clear"); });
    EXPECT_CALL(*route, set_unsaved(false)).Times(1);
    EXPECT_CALL(textures_window_manager, set_texture_storage).Times(1).WillOnce([&](auto) { events.push_back("textures"); });
    EXPECT_CALL(viewer, open(A<const std::weak_ptr<ILevel>&>(), ILevel::OpenMode::Full)).Times(1).WillOnce([&](auto&&...) { events.push_back("viewer"); });

    auto application = register_test_module()
        .with_trlevel_source(trlevel_source)
        .with_viewer(std::move(viewer_ptr))
        .with_route_source([&](auto&&...) {return route; })
        .with_items_window_manager(std::move(items_window_manager_ptr))
        .with_triggers_window_manager(std::move(triggers_window_manager_ptr))
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .with_lights_window_manager(std::move(lights_window_manager_ptr))
        .with_textures_window_manager(std::move(textures_window_manager_ptr))
        .with_camera_sink_window_manager(std::move(camera_sink_window_manager_ptr))
        .with_windows(std::move(windows_ptr))
        .build();
    application->open("test_path.tr2", ILevel::OpenMode::Full);

    ASSERT_TRUE(called.has_value());
    ASSERT_EQ(called.value(), "test_path.tr2");

    ASSERT_EQ(events.back(), "viewer");
}

TEST(Application, PropagatesSettingsWhenUpdated)
{
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, set_settings).Times(2);

    auto application = register_test_module().with_viewer(std::move(viewer_ptr)).build();
    viewer.on_settings({});
}

TEST(Application, SavesSettingsOnShutdown)
{
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    EXPECT_CALL(settings_loader, save_user_settings).Times(1);
    auto application = register_test_module().with_settings_loader(std::move(settings_loader_ptr)).build();
}

TEST(Application, FileOpenedFromCommandLine)
{
    auto startup_options = mock_shared<MockStartupOptions>();
    ON_CALL(*startup_options, filename).WillByDefault(testing::Return("test.tr2"));
    std::optional<std::string> called;
    auto trlevel_source = [&](auto&& filename) { called = filename; return mock_unique<trlevel::mocks::MockLevel>(); };
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, open).Times(1);
    auto application = register_test_module().with_trlevel_source(trlevel_source).with_viewer(std::move(viewer_ptr)).with_startup_options(startup_options).build();
    ASSERT_TRUE(called.has_value());
    ASSERT_EQ(called.value(), "test.tr2");
}

TEST(Application, FileNotOpenedWhenNotSpecified)
{
    int32_t times_called = 0;
    auto trlevel_source = [&](auto&&...) { ++times_called; return mock_unique<trlevel::mocks::MockLevel>(); };
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, open).Times(0);
    auto application = register_test_module().with_trlevel_source(trlevel_source).with_viewer(std::move(viewer_ptr)).build();
    ASSERT_EQ(times_called, 0);
}

TEST(Application, DialogShownOnCloseWithUnsavedRouteBlocksClose)
{
    auto [route_ptr, route] = create_mock<MockRoute>();
    auto dialogs = mock_shared<MockDialogs>();
    auto route_ptr_actual = std::move(route_ptr);

    EXPECT_CALL(route, is_unsaved).WillRepeatedly(Return(true));
    auto application = register_test_module().with_route_source([&](auto&&...) {return std::move(route_ptr_actual); }).with_dialogs(dialogs).build();
    bool closing_called = false;
    auto token = application->on_closing += [&]() { closing_called = true; };
    application->process_message(WM_CLOSE, 0, 0);
    ASSERT_FALSE(closing_called);
}

TEST(Application, DialogShownOnCloseWithUnsavedRouteAllowsClose)
{
    auto [route_ptr, route] = create_mock<MockRoute>();
    auto dialogs = mock_shared<MockDialogs>();
    auto route_ptr_actual = std::move(route_ptr);

    EXPECT_CALL(route, is_unsaved).WillRepeatedly(Return(true));
    EXPECT_CALL(*dialogs, message_box).WillRepeatedly(Return(true));
    auto application = register_test_module().with_route_source([&](auto&&...) {return std::move(route_ptr_actual); }).with_dialogs(dialogs).build();
    bool closing_called = false;
    auto token = application->on_closing += [&]() { closing_called = true; };
    application->process_message(WM_CLOSE, 0, 0);
    ASSERT_TRUE(closing_called);
}

TEST(Application, DialogShownOnOpenWithUnsavedRouteBlocksOpen)
{
    auto [route_ptr, route] = create_mock<MockRoute>();
    auto dialogs = mock_shared<MockDialogs>();
    auto route_ptr_actual = std::move(route_ptr);
    int32_t times_called = 0;
    auto trlevel_source = [&](auto&&...) { ++times_called; return mock_unique<trlevel::mocks::MockLevel>(); };

    EXPECT_CALL(route, is_unsaved).WillRepeatedly(Return(true));
    auto application = register_test_module()
        .with_route_source([&](auto&&...) {return std::move(route_ptr_actual); })
        .with_dialogs(dialogs)
        .with_trlevel_source(trlevel_source)
        .build();
    application->open("", ILevel::OpenMode::Full);

    ASSERT_EQ(times_called, 0);
}

TEST(Application, DialogShownOnOpenWithUnsavedRouteAllowsOpen)
{
    auto [route_ptr, route] = create_mock<MockRoute>();
    auto dialogs = mock_shared<MockDialogs>();
    auto route_ptr_actual = std::move(route_ptr);
    int32_t times_called = 0;
    auto trlevel_source = [&](auto&&...) { ++times_called; return mock_unique<trlevel::mocks::MockLevel>(); };

    EXPECT_CALL(route, is_unsaved).WillRepeatedly(Return(true));
    EXPECT_CALL(*dialogs, message_box).WillRepeatedly(Return(true));
    auto application = register_test_module()
        .with_route_source([&](auto&&...) {return std::move(route_ptr_actual); })
        .with_dialogs(dialogs)
        .with_trlevel_source(trlevel_source)
        .build();
    application->open("", ILevel::OpenMode::Full);

    ASSERT_EQ(times_called, 1);
}

TEST(Application, ClosingEventCalled)
{
    auto application = register_test_module().build();
    bool closing_called = false;
    auto token = application->on_closing += [&]() { closing_called = true; };
    application->process_message(WM_CLOSE, 0, 0);
    ASSERT_TRUE(closing_called);
}


TEST(Application, ExportRouteSavesFile)
{
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, save_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename", 0 }));

    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, save_as).Times(1);
    EXPECT_CALL(*route, set_unsaved(false)).Times(1);

    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_route_source([&](auto&&...) {return route; })
        .with_dialogs(dialogs)
        .build();

    route_window_manager.on_route_save_as();
}

TEST(Application, OpenRouteLoadsFile)
{
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename", 0 }));

    auto [viewer_ptr, viewer] = create_mock <MockViewer>();
    EXPECT_CALL(viewer, set_route).Times(2);
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route_window_manager, set_route).Times(2);
    auto files = mock_shared<MockFiles>();
    EXPECT_CALL(*files, load_file(A<const std::string&>())).Times(1).WillRepeatedly(Return<std::vector<uint8_t>>({ 0x7b, 0x7d }));;

    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_viewer(std::move(viewer_ptr))
        .with_files(files)
        .with_dialogs(dialogs)
        .build();

    route_window_manager.on_route_open();
}

TEST(Application, WindowManagersUpdated)
{
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route_window_manager, update).Times(1);
    auto [items_window_manager_ptr, items_window_manager] = create_mock<MockItemsWindowManager>();
    EXPECT_CALL(items_window_manager, update).Times(1);
    auto [rooms_window_manager_ptr, rooms_window_manager] = create_mock<MockRoomsWindowManager>();
    EXPECT_CALL(rooms_window_manager, update).Times(1);
    auto [triggers_window_manager_ptr, triggers_window_manager] = create_mock<MockTriggersWindowManager>();
    EXPECT_CALL(triggers_window_manager, update).Times(1);
    auto [lights_window_manager_ptr, lights_window_manager] = create_mock<MockLightsWindowManager>();
    EXPECT_CALL(lights_window_manager, update).Times(1);
    auto [windows_ptr, windows] = create_mock<MockWindows>();
    EXPECT_CALL(windows, update).Times(1);

    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_items_window_manager(std::move(items_window_manager_ptr))
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .with_triggers_window_manager(std::move(triggers_window_manager_ptr))
        .with_lights_window_manager(std::move(lights_window_manager_ptr))
        .with_windows(std::move(windows_ptr))
        .build();
    application->render();
}

TEST(Application, WindowManagersAndViewerRendered)
{
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route_window_manager, render).Times(1);
    auto [items_window_manager_ptr, items_window_manager] = create_mock<MockItemsWindowManager>();
    EXPECT_CALL(items_window_manager, render).Times(1);
    auto [rooms_window_manager_ptr, rooms_window_manager] = create_mock<MockRoomsWindowManager>();
    EXPECT_CALL(rooms_window_manager, render).Times(1);
    auto [triggers_window_manager_ptr, triggers_window_manager] = create_mock<MockTriggersWindowManager>();
    EXPECT_CALL(triggers_window_manager, render).Times(1);
    auto [lights_window_manager_ptr, lights_window_manager] = create_mock<MockLightsWindowManager>();
    EXPECT_CALL(lights_window_manager, render).Times(1);
    auto [log_window_manager_ptr, log_window_manager] = create_mock<MockLogWindowManager>();
    EXPECT_CALL(log_window_manager, render).Times(1);
    auto [textures_window_manager_ptr, textures_window_manager] = create_mock<MockTexturesWindowManager>();
    EXPECT_CALL(textures_window_manager, render).Times(1);
    auto [camera_sink_window_manager_ptr, camera_sink_window_manager] = create_mock<MockCameraSinkWindowManager>();
    EXPECT_CALL(camera_sink_window_manager, render).Times(1);
    auto [console_manager_ptr, console_manager] = create_mock<MockConsoleManager>();
    EXPECT_CALL(console_manager, render).Times(1);
    auto [plugins_window_manager_ptr, plugins_window_manager] = create_mock<MockPluginsWindowManager>();
    EXPECT_CALL(plugins_window_manager, render).Times(1);
    auto [windows_ptr, windows] = create_mock<MockWindows>();
    EXPECT_CALL(windows, render).Times(1);
    auto plugins = mock_shared<MockPlugins>();
    EXPECT_CALL(*plugins, render_ui).Times(1);

    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, render).Times(1);

    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_items_window_manager(std::move(items_window_manager_ptr))
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .with_triggers_window_manager(std::move(triggers_window_manager_ptr))
        .with_lights_window_manager(std::move(lights_window_manager_ptr))
        .with_log_window_manager(std::move(log_window_manager_ptr))
        .with_textures_window_manager(std::move(textures_window_manager_ptr))
        .with_camera_sink_window_manager(std::move(camera_sink_window_manager_ptr))
        .with_console_manager(std::move(console_manager_ptr))
        .with_plugins_window_manager(std::move(plugins_window_manager_ptr))
        .with_windows(std::move(windows_ptr))
        .with_viewer(std::move(viewer_ptr))
        .with_plugins(plugins)
        .build();
    application->render();
}

TEST(Application, WaypointReorderedMovesWaypoint)
{
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, move(1, 2)).Times(1);

    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_route_source([&](auto&&...) {return route; })
        .build();

    route_window_manager.on_waypoint_reordered(1, 2);
}

TEST(Application, MapColoursSetOnRoomWindow)
{
    auto [rooms_window_manager_ptr, rooms_window_manager] = create_mock<MockRoomsWindowManager>();
    EXPECT_CALL(rooms_window_manager, set_map_colours).Times(1);

    auto application = register_test_module()
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .build();
}

TEST(Application, MapColoursSetOnSettingsChanged)
{
    auto [rooms_window_manager_ptr, rooms_window_manager] = create_mock<MockRoomsWindowManager>();
    EXPECT_CALL(rooms_window_manager, set_map_colours).Times(2);
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();

    auto application = register_test_module()
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .with_viewer(std::move(viewer_ptr))
        .build();

    viewer.on_settings(UserSettings());
}

TEST(Application, ResetFonts)
{
    auto fonts = mock_shared<MockFonts>();
    EXPECT_CALL(*fonts, add_font(std::string("Console"), 
        testing::AllOf(testing::Field(&FontSetting::name, testing::Eq("Consolas")),
                       testing::Field(&FontSetting::filename, testing::Eq("consola.ttf")),
                       testing::Field(&FontSetting::size, testing::Eq(12)))));
    EXPECT_CALL(*fonts, add_font(std::string("Default"),
        testing::AllOf(testing::Field(&FontSetting::name, testing::Eq("Arial")),
            testing::Field(&FontSetting::filename, testing::Eq("arial.ttf")),
            testing::Field(&FontSetting::size, testing::Eq(12)))));

    auto application = register_test_module()
        .with_fonts(fonts)
        .build();

    application->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_RESET_FONTS, 0), 0);
}

TEST(Application, ResetLayout)
{
    auto imgui_backend = mock_shared<MockImGuiBackend>();
    EXPECT_CALL(*imgui_backend, reset_layout);

    auto application = register_test_module()
        .with_imgui_backend(imgui_backend)
        .build();

    application->process_message(WM_COMMAND, MAKEWPARAM(ID_WINDOWS_RESET_LAYOUT, 0), 0);
}

TEST(Application, RoomsWindowRoomVisibilityCaptured)
{
    auto [rooms_window_manager_ptr, rooms_window_manager] = create_mock<MockRoomsWindowManager>();
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    EXPECT_CALL(level, set_room_visibility(0, true)).Times(1);

    ILevel::Source level_source = [&](auto&&...) { return std::move(level_ptr); };

    auto application = register_test_module()
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .with_level_source(level_source)
        .build();

    application->open("", ILevel::OpenMode::Full);

    auto room = mock_shared<MockRoom>();
    rooms_window_manager.on_room_visibility(room, true);
}

TEST(Application, ViewerRoomVisibilityCaptured)
{
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    EXPECT_CALL(level, set_room_visibility(0, true)).Times(1);

    ILevel::Source level_source = [&](auto&&...) { return std::move(level_ptr); };

    auto application = register_test_module()
        .with_viewer(std::move(viewer_ptr))
        .with_level_source(level_source)
        .build();

    application->open("", ILevel::OpenMode::Full);

    auto room = mock_shared<MockRoom>();
    viewer.on_room_visibility(room, true);
}

TEST(Application, LevelLoadedOnReload)
{
    auto [file_menu_ptr, file_menu] = create_mock<MockFileMenu>();
    std::vector<std::string> called;
    auto trlevel_source = [&](auto&& filename)-> std::unique_ptr<trlevel::ILevel> { called.push_back(filename); return mock_unique<trlevel::mocks::MockLevel>(); };
    ILevel::Source level_source = [](auto&&...) 
    { 
        auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
        ON_CALL(level, filename).WillByDefault(Return("reload.tr2"));
        return std::move(level_ptr);
    };
    auto application = register_test_module()
        .with_trlevel_source(trlevel_source)
        .with_level_source(level_source)
        .with_file_menu(std::move(file_menu_ptr))
        .build();
    file_menu.on_file_open("test_path.tr2");
    file_menu.on_reload();
    ASSERT_EQ(called.size(), 2);

    std::vector<std::string> expected{ "test_path.tr2", "reload.tr2" };
    ASSERT_EQ(called, expected);
}

TEST(Application, ReloadSyncsProperties)
{
    using DirectX::SimpleMath::Vector3;

    std::vector<std::string> called;

    auto [original_ptr, original] = create_mock<trview::mocks::MockLevel>();
    auto [reloaded_ptr, reloaded] = create_mock<trview::mocks::MockLevel>();

    std::vector<std::shared_ptr<IItem>> items;
    for (int i = 0; i < 5; ++i) 
    {
        items.push_back(mock_shared<MockItem>()->with_number(i));
    }
    std::vector<std::weak_ptr<IItem>> items_weak{ std::from_range, items };
    
    std::vector<std::shared_ptr<ITrigger>> triggers;
    for (int i = 0; i < 5; ++i)
    {
        auto trigger = mock_shared<MockTrigger>();
        ON_CALL(*trigger, number).WillByDefault(Return(i));
        triggers.push_back(trigger);
    }
    std::vector<std::weak_ptr<ITrigger>> triggers_weak;
    for (const auto& t : triggers)
    {
        triggers_weak.push_back(t);
    }

    std::vector<std::shared_ptr<ILight>> lights;
    for (int i = 0; i < 5; ++i)
    {
        auto light = mock_shared<MockLight>();
        ON_CALL(*light, number).WillByDefault(Return(i));
        lights.push_back(light);
    }
    std::vector<std::weak_ptr<ILight>> lights_weak;
    for (const auto& l : lights)
    {
        lights_weak.push_back(l);
    }

    auto original_room = mock_shared<MockRoom>()->with_number(3);
    auto room = mock_shared<MockRoom>()->with_number(3);

    ON_CALL(original, items).WillByDefault(Return(items_weak));
    ON_CALL(original, selected_item).WillByDefault(Return(3));
    ON_CALL(original, triggers).WillByDefault(Return(triggers_weak));
    ON_CALL(original, selected_trigger).WillByDefault(Return(3));
    ON_CALL(original, lights).WillByDefault(Return(lights_weak));
    ON_CALL(original, selected_light).WillByDefault(Return(3));
    ON_CALL(original, number_of_rooms).WillByDefault(Return(5));
    ON_CALL(original, selected_room).WillByDefault(Return(original_room));

    ON_CALL(reloaded, items).WillByDefault(Return(items_weak));
    EXPECT_CALL(reloaded, set_selected_item(3)).Times(1);
    ON_CALL(reloaded, triggers).WillByDefault(Return(triggers_weak));
    EXPECT_CALL(reloaded, set_selected_trigger(3)).Times(1);
    ON_CALL(reloaded, lights).WillByDefault(Return(lights_weak));
    EXPECT_CALL(reloaded, set_selected_light(3)).Times(1);
    ON_CALL(reloaded, number_of_rooms).WillByDefault(Return(5));
    const auto matcher = [](auto r) { return std::get<0>(r).lock(); };
    EXPECT_CALL(reloaded, set_selected_room).With(ResultOf(matcher, Eq(std::shared_ptr<IRoom>{}))).Times(AnyNumber());
    EXPECT_CALL(reloaded, set_selected_room).With(ResultOf(matcher, Eq(room))).Times(1);
    EXPECT_CALL(reloaded, trigger).WillRepeatedly(Return(triggers_weak[3]));
    EXPECT_CALL(reloaded, light).WillRepeatedly(Return(lights_weak[3]));
    EXPECT_CALL(reloaded, item).WillRepeatedly(Return(items[3]));
    EXPECT_CALL(reloaded, room(3)).WillRepeatedly(Return(room));

    std::list<std::unique_ptr<ILevel>> levels;
    levels.push_back(std::move(original_ptr));
    levels.push_back(std::move(reloaded_ptr));

    ILevel::Source level_source = [&](auto&&...)
    {
        auto level = std::move(levels.front());
        levels.pop_front();
        return std::move(level);
    };

    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    ON_CALL(viewer, target).WillByDefault(Return(Vector3(1, 2, 3)));
    EXPECT_CALL(viewer, set_target(Vector3(1, 2, 3))).Times(1);

    auto application = register_test_module()
        .with_level_source(level_source)
        .with_viewer(std::move(viewer_ptr))
        .build();

    application->open("test.tr2", ILevel::OpenMode::Full);
    application->open("test.tr2", ILevel::OpenMode::Reload);
}

TEST(Application, RouteSetToDefaultColoursOnReset)
{
    UserSettings settings;
    settings.route_colour = Colour::Yellow;
    settings.waypoint_colour = Colour::Cyan;
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, set_colour(Colour::Yellow)).Times(1);
    EXPECT_CALL(*route, set_waypoint_colour(Colour::Cyan)).Times(1);
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    ON_CALL(settings_loader, load_user_settings).WillByDefault(Return(settings));
    auto application = register_test_module()
        .with_settings_loader(std::move(settings_loader_ptr))
        .with_route_source([&](auto&&...) {return route; })
        .build();
    application->open("test.tr2", ILevel::OpenMode::Full);
}

TEST(Application, RouteWindowCreatedOnStartup)
{
    UserSettings settings;
    settings.route_startup = true;
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    ON_CALL(settings_loader, load_user_settings).WillByDefault(Return(settings));
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route_window_manager, create_window).Times(1);
    auto application = register_test_module()
        .with_settings_loader(std::move(settings_loader_ptr))
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .build();
}

TEST(Application, RouteWindowNotCreatedOnStartup)
{
    UserSettings settings;
    settings.route_startup = false;
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    ON_CALL(settings_loader, load_user_settings).WillByDefault(Return(settings));
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route_window_manager, create_window).Times(0);
    auto application = register_test_module()
        .with_settings_loader(std::move(settings_loader_ptr))
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .build();
}

TEST(Application, RecentRouteLoaded)
{
    UserSettings settings;
    settings.recent_routes["test.tr2"] = { "test.tvr", false };
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    ON_CALL(settings_loader, load_user_settings).WillByDefault(Return(settings));
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, message_box(std::wstring(L"Reopen last used route for this level?"), std::wstring(L"Reopen route"), IDialogs::Buttons::Yes_No)).Times(1).WillOnce(Return(true));
    auto route = mock_shared<MockRoute>();
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, set_route(std::shared_ptr<IRoute>(route))).Times(2);
    UserSettings called_settings{};
    EXPECT_CALL(viewer, set_settings).Times(AtLeast(1)).WillRepeatedly(SaveArg<0>(&called_settings));
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    ON_CALL(level, filename).WillByDefault(Return("test.tr2"));
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    ON_CALL(route_window_manager, is_window_open).WillByDefault(Return(true));

    auto application = register_test_module()
        .with_settings_loader(std::move(settings_loader_ptr))
        .with_dialogs(dialogs)
        .with_viewer(std::move(viewer_ptr))
        .with_route_source([&](auto&&...) {return route; })
        .with_level_source([&](auto&&...) { return std::move(level_ptr); })
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .build();
    
    application->open("test.tr2", ILevel::OpenMode::Full);

    ASSERT_EQ(called_settings.recent_routes["test.tr2"].route_path, "test.tvr");
}

TEST(Application, RecentRouteNotLoaded)
{
    UserSettings settings;
    settings.recent_routes["test.tr2"] = { "test.tvr", false };
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    ON_CALL(settings_loader, load_user_settings).WillByDefault(Return(settings));
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, message_box(std::wstring(L"Reopen last used route for this level?"), std::wstring(L"Reopen route"), IDialogs::Buttons::Yes_No)).Times(1).WillOnce(Return(false));
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    UserSettings called_settings{};
    EXPECT_CALL(viewer, set_settings).Times(AtLeast(1)).WillRepeatedly(SaveArg<0>(&called_settings));
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    ON_CALL(level, filename).WillByDefault(Return("test.tr2"));
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    ON_CALL(route_window_manager, is_window_open).WillByDefault(Return(true));

    auto application = register_test_module()
        .with_settings_loader(std::move(settings_loader_ptr))
        .with_dialogs(dialogs)
        .with_viewer(std::move(viewer_ptr))
        .with_level_source([&](auto&&...) { return std::move(level_ptr); })
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .build();

    application->open("test.tr2", ILevel::OpenMode::Full);

    ASSERT_TRUE(called_settings.recent_routes.empty());
}

TEST(Application, RecentRouteLoadedOnWindowOpened)
{
    UserSettings settings;
    settings.recent_routes["test.tr2"] = { "test.tvr", false };
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    ON_CALL(settings_loader, load_user_settings).WillByDefault(Return(settings));
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, message_box(std::wstring(L"Reopen last used route for this level?"), std::wstring(L"Reopen route"), IDialogs::Buttons::Yes_No)).Times(1).WillOnce(Return(true));
    auto route = mock_shared<MockRoute>();
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, set_route(std::shared_ptr<IRoute>(route))).Times(2);
    UserSettings called_settings{};
    EXPECT_CALL(viewer, set_settings).Times(AtLeast(1)).WillRepeatedly(SaveArg<0>(&called_settings));
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    ON_CALL(level, filename).WillByDefault(Return("test.tr2"));
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();

    auto application = register_test_module()
        .with_settings_loader(std::move(settings_loader_ptr))
        .with_dialogs(dialogs)
        .with_viewer(std::move(viewer_ptr))
        .with_route_source([&](auto&&...) {return route; })
        .with_level_source([&](auto&&...) { return std::move(level_ptr); })
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .build();

    application->open("test.tr2", ILevel::OpenMode::Full);

    ON_CALL(route_window_manager, is_window_open).WillByDefault(Return(true));
    route_window_manager.on_window_created();

    ASSERT_EQ(called_settings.recent_routes["test.tr2"].route_path, "test.tvr");
}

TEST(Application, SectorHoverForwarded)
{
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, select_sector).Times(1);

    auto [rooms_window_manager_ptr, rooms_window_manager] = create_mock<MockRoomsWindowManager>();

    auto application = register_test_module()
        .with_viewer(std::move(viewer_ptr))
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .build();

    rooms_window_manager.on_sector_hover(mock_shared<MockSector>());
}

TEST(Application, WaypointChangedUpdatesViewer)
{
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, set_scene_changed).Times(1);

    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();

    auto application = register_test_module()
        .with_viewer(std::move(viewer_ptr))
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .build();

    route_window_manager.on_waypoint_changed();
}

TEST(Application, ViewerUpdatedWhenCameraSinkTypeChanges)
{
    auto [camera_sink_window_manager_ptr, camera_sink_window_manager] = create_mock<MockCameraSinkWindowManager>();
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, set_scene_changed).Times(1);

    auto application = register_test_module()
        .with_camera_sink_window_manager(std::move(camera_sink_window_manager_ptr))
        .with_viewer(std::move(viewer_ptr))
        .build();

    camera_sink_window_manager.on_camera_sink_type_changed();
}

TEST(Application, CameraSinksWindowCreatedOnStartup)
{
    UserSettings settings;
    settings.camera_sink_startup = true;
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    ON_CALL(settings_loader, load_user_settings).WillByDefault(Return(settings));
    auto [camera_sink_window_manager_ptr, camera_sink_window_manager] = create_mock<MockCameraSinkWindowManager>();
    EXPECT_CALL(camera_sink_window_manager, create_window).Times(1);
    auto application = register_test_module()
        .with_settings_loader(std::move(settings_loader_ptr))
        .with_camera_sink_window_manager(std::move(camera_sink_window_manager_ptr))
        .build();
}

TEST(Application, CameraSinkSelectedEventForwarded)
{
    auto [camera_sink_window_manager_ptr, camera_sink_window_manager] = create_mock<MockCameraSinkWindowManager>();
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    EXPECT_CALL(level, set_selected_camera_sink).Times(1);

    auto application = register_test_module()
        .with_camera_sink_window_manager(std::move(camera_sink_window_manager_ptr))
        .with_level_source([&](auto&&...) { return std::move(level_ptr); })
        .build();

    application->open("test_path.tr2", ILevel::OpenMode::Full);
    camera_sink_window_manager.on_camera_sink_selected(mock_shared<MockCameraSink>());
}

TEST(Application, CameraSinkVisibilityEventForwarded)
{
    auto [camera_sink_window_manager_ptr, camera_sink_window_manager] = create_mock<MockCameraSinkWindowManager>();
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    EXPECT_CALL(level, set_camera_sink_visibility).Times(1);

    auto application = register_test_module()
        .with_camera_sink_window_manager(std::move(camera_sink_window_manager_ptr))
        .with_level_source([&](auto&&...) { return std::move(level_ptr); })
        .build();

    application->open("test_path.tr2", ILevel::OpenMode::Full);
    camera_sink_window_manager.on_camera_sink_visibility(mock_shared<MockCameraSink>(), true);
}

TEST(Application, TriggerSelectedFromCameraSinkWindow)
{
    auto [camera_sink_window_manager_ptr, camera_sink_window_manager] = create_mock<MockCameraSinkWindowManager>();
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    EXPECT_CALL(level, set_selected_trigger).Times(1);

    auto application = register_test_module()
        .with_camera_sink_window_manager(std::move(camera_sink_window_manager_ptr))
        .with_level_source([&](auto&&...) { return std::move(level_ptr); })
        .build();

    application->open("test_path.tr2", ILevel::OpenMode::Full);
    camera_sink_window_manager.on_trigger_selected(mock_shared<MockTrigger>());
}

TEST(Application, CameraSinkSelectedFromTriggersWindow)
{
    auto sink = mock_shared<MockCameraSink>();
    auto [triggers_window_manager_ptr, triggers_window_manager] = create_mock<MockTriggersWindowManager>();
    auto [level_ptr, level] = create_mock<trview::mocks::MockLevel>();
    EXPECT_CALL(level, set_selected_camera_sink).Times(1);
    ON_CALL(level, camera_sinks).WillByDefault(testing::Return(std::vector<std::weak_ptr<ICameraSink>>{ sink }));

    auto application = register_test_module()
        .with_triggers_window_manager(std::move(triggers_window_manager_ptr))
        .with_level_source([&](auto&&...) { return std::move(level_ptr); })
        .build();

    application->open("test_path.tr2", ILevel::OpenMode::Full);
    triggers_window_manager.on_camera_sink_selected(0);
}

TEST(Application, SetCurrentLevelPrompt)
{
    auto level = mock_shared<trview::mocks::MockLevel>();
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, message_box).Times(0);

    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, open).Times(1);

    auto application = register_test_module()
        .with_dialogs(dialogs)
        .with_viewer(std::move(viewer_ptr))
        .build();

    application->set_current_level(level, ILevel::OpenMode::Full, true);
}

TEST(Application, SetCurrentLevelNoPrompt)
{
    auto level = mock_shared<trview::mocks::MockLevel>();
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, message_box).Times(0);

    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, open).Times(1);

    auto application = register_test_module()
        .with_dialogs(dialogs)
        .with_viewer(std::move(viewer_ptr))
        .build();

    application->set_current_level(level, ILevel::OpenMode::Full, false);
}

TEST(Application, PluginsInitialised)
{
    auto plugins = mock_shared<MockPlugins>();
    EXPECT_CALL(*plugins, initialise).Times(1);

    auto application = register_test_module()
        .with_plugins(plugins)
        .build();
}

TEST(Application, LocalLevels)
{
    std::vector<std::string> files{ "test", "test2" };
    auto [file_menu_ptr, file_menu] = create_mock<MockFileMenu>();
    EXPECT_CALL(file_menu, local_levels).WillRepeatedly(Return(files));

    auto application = register_test_module()
        .with_file_menu(std::move(file_menu_ptr))
        .build();

    auto result = application->local_levels();
    ASSERT_EQ(result, files);
}

TEST(Application, RouteOpen)
{
    UserSettings settings{ .randomizer_tools = true };
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    ON_CALL(settings_loader, load_user_settings).WillByDefault(Return(settings));

    auto files = mock_shared<MockFiles>();
    ON_CALL(*files, load_file(A<const std::string&>())).WillByDefault(Return(std::vector<uint8_t>{0x7B, 0x7D}));

    auto route = mock_shared<MockRoute>();
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto dialogs = mock_shared<MockDialogs>();
    ON_CALL(*dialogs, open_file).WillByDefault(Return(IDialogs::FileResult{.filename = "test", .filter_index = 1 }));
    auto application = register_test_module()
        .with_settings_loader(std::move(settings_loader_ptr))
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_route_source([=](auto&&...) { return route; })
        .with_dialogs(dialogs)
        .with_files(files)
        .build();

    route_window_manager.on_route_open();

    ASSERT_EQ(application->route(), route);
}

TEST(Application, RouteOpenRandomizer)
{
    UserSettings settings{ .randomizer_tools = false };
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    ON_CALL(settings_loader, load_user_settings).WillByDefault(Return(settings));

    auto files = mock_shared<MockFiles>();
    ON_CALL(*files, load_file(A<const std::string&>())).WillByDefault(Return(std::vector<uint8_t>{0x7B, 0x7D}));

    auto route = mock_shared<MockRandomizerRoute>();
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto dialogs = mock_shared<MockDialogs>();
    ON_CALL(*dialogs, open_file).WillByDefault(Return(IDialogs::FileResult{.filename = "test.json", .filter_index = 2 }));
    auto application = register_test_module()
        .with_settings_loader(std::move(settings_loader_ptr))
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_randomizer_route_source([=](auto&&...) { return route; })
        .with_dialogs(dialogs)
        .with_files(files)
        .build();

    route_window_manager.on_route_open();

    ASSERT_EQ(application->route(), route);
}

TEST(Application, RouteReload)
{
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, reload).Times(1);

    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_route_source([=](auto&&...) { return route; })
        .build();

    route_window_manager.on_route_reload();
}

TEST(Application, RouteSave)
{
    auto route = mock_shared<MockRoute>();
    ON_CALL(*route, filename).WillByDefault(Return("test"));
    EXPECT_CALL(*route, save).Times(1);
    EXPECT_CALL(*route, set_filename).Times(0);

    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_route_source([=](auto&&...) { return route; })
        .build();

    route_window_manager.on_route_save();
}

TEST(Application, RouteSaveNoFilename)
{
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, save_as).Times(1);
    EXPECT_CALL(*route, set_filename).Times(1);
    EXPECT_CALL(*route, set_unsaved(false)).Times(1);

    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto dialogs = mock_shared<MockDialogs>();
    ON_CALL(*dialogs, save_file).WillByDefault(Return(IDialogs::FileResult{ .filename = "test", .filter_index = 1 }));

    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_route_source([=](auto&&...) { return route; })
        .with_dialogs(dialogs)
        .build();

    route_window_manager.on_route_save();
}

TEST(Application, RouteSaveAs)
{
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, save_as).Times(1);
    EXPECT_CALL(*route, set_filename).Times(1);
    EXPECT_CALL(*route, set_unsaved(false)).Times(1);

    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto dialogs = mock_shared<MockDialogs>();
    ON_CALL(*dialogs, save_file).WillByDefault(Return(IDialogs::FileResult{.filename = "test", .filter_index = 1 }));

    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_route_source([=](auto&&...) { return route; })
        .with_dialogs(dialogs)
        .build();

    route_window_manager.on_route_save_as();
}

TEST(Application, RouteLevelSwitch)
{
    auto [file_menu_ptr, file_menu] = create_mock<MockFileMenu>();
    EXPECT_CALL(file_menu, switch_to("test1"));

    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto application = register_test_module()
        .with_file_menu(std::move(file_menu_ptr))
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .build();
      
    route_window_manager.on_level_switch("test1");
}

TEST(Application, RouteNewRoute)
{
    std::size_t index = 0;
    std::vector<std::shared_ptr<MockRoute>> routes
    {
        mock_shared<MockRoute>(),
        mock_shared<MockRoute>()
    };

    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route_window_manager, set_route).Times(2);
    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_route_source([&](auto&&...) -> std::shared_ptr<IRoute>
            {
                if (index < routes.size())
                {
                    return routes[index++];
                }
                return {};
            })
        .build();

    ASSERT_EQ(application->route(), routes[0]);
    route_window_manager.on_new_route();
    ASSERT_EQ(application->route(), routes[1]);
}

TEST(Application, RouteNewRandomizerRoute)
{
    auto route = mock_shared<MockRandomizerRoute>();
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route_window_manager, set_route).Times(2);
    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_randomizer_route_source([&](auto&&...) { return route; })
        .build();

    ASSERT_NE(application->route(), route);
    route_window_manager.on_new_randomizer_route();
    ASSERT_EQ(application->route(), route);
}

TEST(Application, OnStaticMeshSelected)
{
    auto level = mock_shared<trview::mocks::MockLevel>();
    auto static_mesh = mock_shared<MockStaticMesh>();
    auto [rooms_window_manager_ptr, rooms_window_manager] = create_mock<MockRoomsWindowManager>();
    auto [windows_ptr, windows] = create_mock<MockWindows>();
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    auto application = register_test_module()
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .with_windows(std::move(windows_ptr))
        .with_viewer(std::move(viewer_ptr))
        .build();

    application->set_current_level(level, ILevel::OpenMode::Full, false);

    EXPECT_CALL(viewer, select_static_mesh).Times(1);
    EXPECT_CALL(windows, select).Times(1);
    rooms_window_manager.on_static_mesh_selected(static_mesh);
}

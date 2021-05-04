#include <trview.app/Application.h>
#include <trview.app/Mocks/Menus/IUpdateChecker.h>
#include <trview.app/Mocks/Menus/IFileDropper.h>
#include <trview.app/Mocks/Menus/ILevelSwitcher.h>
#include <trview.app/Mocks/Menus/IRecentFiles.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trview.app/Mocks/Settings/ISettingsLoader.h>
#include <trview.app/Mocks/Settings/IStartupOptions.h>
#include <trview.app/Mocks/Windows/IViewer.h>
#include <trview.app/Mocks/Windows/IItemsWindowManager.h>
#include <trview.app/Mocks/Windows/ITriggersWindowManager.h>
#include <trview.app/Mocks/Windows/IRouteWindowManager.h>
#include <trview.app/Mocks/Windows/IRoomsWindowManager.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trlevel/Mocks/ILevelLoader.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <external/boost/di.hpp>

using namespace trview;
using namespace trview::tests;
using namespace testing;
using namespace trview::mocks;
using namespace trlevel::mocks;
using testing::_;
using namespace boost;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module(std::unique_ptr<IUpdateChecker> update_checker = nullptr, std::unique_ptr<ISettingsLoader> settings_loader = nullptr, std::unique_ptr<IFileDropper> file_dropper = nullptr,
        std::unique_ptr<trlevel::ILevelLoader> level_loader = nullptr, std::unique_ptr<ILevelSwitcher> level_switcher = nullptr, std::unique_ptr<IRecentFiles> recent_files = nullptr,
        std::unique_ptr<IViewer> viewer = nullptr, std::unique_ptr<IRoute> route = nullptr, std::unique_ptr<IItemsWindowManager> items_window_manager = nullptr, std::unique_ptr<ITriggersWindowManager> triggers_window_manager = nullptr,
        std::unique_ptr<IRouteWindowManager> route_window_manager = nullptr, std::unique_ptr<IRoomsWindowManager> rooms_window_manager = nullptr,
        std::shared_ptr<IStartupOptions> startup_options = nullptr)
    {
        choose_mock<MockUpdateChecker>(update_checker);
        choose_mock<MockSettingsLoader>(settings_loader);
        choose_mock<MockFileDropper>(file_dropper);
        choose_mock<MockLevelLoader>(level_loader);
        choose_mock<MockLevelSwitcher>(level_switcher);
        choose_mock<MockRecentFiles>(recent_files);
        choose_mock<MockViewer>(viewer);
        choose_mock<MockRoute>(route);
        choose_mock<MockItemsWindowManager>(items_window_manager);
        choose_mock<MockTriggersWindowManager>(triggers_window_manager);
        choose_mock<MockRouteWindowManager>(route_window_manager);
        choose_mock<MockRoomsWindowManager>(rooms_window_manager);
        choose_mock<MockStartupOptions>(startup_options);

        auto shortcuts = std::make_shared<MockShortcuts>();
        EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });

        return di::make_injector(
            di::bind<Window>.to(create_test_window(L"ApplicationTests")),
            di::bind<IUpdateChecker>.to([&](auto&&) { return std::move(update_checker); }),
            di::bind<ISettingsLoader>.to([&](auto&&) { return std::move(settings_loader); }),
            di::bind<IFileDropper>.to([&](auto&&) { return std::move(file_dropper); }),
            di::bind<trlevel::ILevelLoader>.to([&](auto&&) { return std::move(level_loader); }),
            di::bind<ILevelSwitcher>.to([&](auto&&) { return std::move(level_switcher); }),
            di::bind<IRecentFiles>.to([&](auto&&) { return std::move(recent_files); }),
            di::bind<IViewer>.to([&](auto&&) { return std::move(viewer); }),
            di::bind<IRoute::Source>.to([&](const auto& injector)->IRoute::Source { return [&]() { return std::move(route); }; }),
            di::bind<IShortcuts>.to(shortcuts),
            di::bind<IItemsWindowManager>.to([&](auto&&) { return std::move(items_window_manager); }),
            di::bind<ITriggersWindowManager>.to([&](auto&&) { return std::move(triggers_window_manager); }),
            di::bind<IRouteWindowManager>.to([&](auto&&) { return std::move(route_window_manager); }),
            di::bind<IRoomsWindowManager>.to([&](auto&&) { return std::move(rooms_window_manager); }),
            di::bind<ILevel::Source>.to([](const auto& injector)->ILevel::Source { return [](auto) { return std::make_unique<trview::mocks::MockLevel>(); }; }),
            di::bind<IStartupOptions>.to(startup_options)
        ).create<std::unique_ptr<Application>>();
    }
}

TEST(Application, ChecksForUpdates)
{
    auto [update_checker_ptr, update_checker] = create_mock<MockUpdateChecker>();
    EXPECT_CALL(update_checker, check_for_updates).Times(1);
    auto application = register_test_module(std::move(update_checker_ptr));
}

TEST(Application, SettingsLoadedAndSaved)
{
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    EXPECT_CALL(settings_loader, load_user_settings).Times(1);
    EXPECT_CALL(settings_loader, save_user_settings).Times(1);
    auto application = register_test_module(nullptr, std::move(settings_loader_ptr));
}

TEST(Application, FileDropperOpensFile)
{
    auto [file_dropper_ptr, file_dropper] = create_mock<MockFileDropper>();
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    EXPECT_CALL(level_loader, load_level("test_path.tr2")).Times(1).WillRepeatedly(Throw(std::exception()));
    auto application = register_test_module(nullptr, nullptr, std::move(file_dropper_ptr), std::move(level_loader_ptr));
    file_dropper.on_file_dropped("test_path.tr2");
}

TEST(Application, LevelLoadedOnSwitchLevel)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [level_switcher_ptr, level_switcher] = create_mock<MockLevelSwitcher>();
    EXPECT_CALL(level_loader, load_level("test_path.tr2")).Times(1).WillRepeatedly(Throw(std::exception()));
    auto application = register_test_module(nullptr, nullptr, nullptr, std::move(level_loader_ptr), std::move(level_switcher_ptr));
    level_switcher.on_switch_level("test_path.tr2");
}

TEST(Application, LevelLoadedOnRecentFileOpen)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [recent_files_ptr, recent_files] = create_mock<MockRecentFiles>();
    EXPECT_CALL(level_loader, load_level("test_path.tr2")).Times(1).WillRepeatedly(Throw(std::exception()));
    auto application = register_test_module(nullptr, nullptr, nullptr, std::move(level_loader_ptr), nullptr, std::move(recent_files_ptr));
    recent_files.on_file_open("test_path.tr2");
}

TEST(Application, RecentFilesUpdatedOnFileOpen)
{
    auto [recent_files_ptr, recent_files] = create_mock<MockRecentFiles>();
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    EXPECT_CALL(recent_files, set_recent_files(std::list<std::string>{})).Times(1);
    EXPECT_CALL(recent_files, set_recent_files(std::list<std::string>{"test_path.tr2"})).Times(1);
    EXPECT_CALL(level_loader, load_level("test_path.tr2")).WillOnce(Return(ByMove(std::make_unique<trlevel::mocks::MockLevel>())));
    auto application = register_test_module(nullptr, nullptr, nullptr, std::move(level_loader_ptr), nullptr, std::move(recent_files_ptr));
    application->open("test_path.tr2");
}

TEST(Application, FileOpenedInViewer)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    auto [level_ptr, level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(level_loader, load_level("test_path.tr2")).WillOnce(Return(ByMove(std::move(level_ptr))));
    EXPECT_CALL(viewer, open(NotNull())).Times(1);
    auto application = register_test_module(nullptr, nullptr, nullptr, std::move(level_loader_ptr), nullptr, nullptr, std::move(viewer_ptr));
    application->open("test_path.tr2");
}

TEST(Application, WindowContentsResetBeforeViewerLoaded)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    auto [level_ptr, level] = create_mock<trlevel::mocks::MockLevel>();
    auto [items_window_manager_ptr, items_window_manager] = create_mock<MockItemsWindowManager>();
    auto [rooms_window_manager_ptr, rooms_window_manager] = create_mock<MockRoomsWindowManager>();
    auto [triggers_window_manager_ptr, triggers_window_manager] = create_mock<MockTriggersWindowManager>();
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto [route_ptr, route] = create_mock<MockRoute>();

    std::vector<std::string> events;

    EXPECT_CALL(level_loader, load_level("test_path.tr2")).WillOnce(Return(ByMove(std::move(level_ptr))));
    
    EXPECT_CALL(items_window_manager, set_items(A<const std::vector<Item>&>())).Times(1).WillOnce([&](auto) { events.push_back("items_items"); });
    EXPECT_CALL(items_window_manager, set_triggers(A<const std::vector<Trigger*>&>())).Times(1).WillOnce([&](auto) { events.push_back("items_triggers"); });
    EXPECT_CALL(triggers_window_manager, set_items(A<const std::vector<Item>&>())).Times(1).WillOnce([&](auto) { events.push_back("triggers_items"); });
    EXPECT_CALL(triggers_window_manager, set_triggers(A<const std::vector<Trigger*>&>())).Times(1).WillOnce([&](auto) { events.push_back("triggers_triggers"); });
    EXPECT_CALL(rooms_window_manager, set_items(A<const std::vector<Item>&>())).Times(1).WillOnce([&](auto) { events.push_back("rooms_items"); });
    EXPECT_CALL(rooms_window_manager, set_triggers(A<const std::vector<Trigger*>&>())).Times(1).WillOnce([&](auto) { events.push_back("rooms_triggers"); });
    EXPECT_CALL(rooms_window_manager, set_rooms(A<const std::vector<std::weak_ptr<IRoom>>&>())).Times(1).WillOnce([&](auto) { events.push_back("rooms_rooms"); });
    EXPECT_CALL(route_window_manager, set_items(A<const std::vector<Item>&>())).Times(1).WillOnce([&](auto) { events.push_back("route_items"); });
    EXPECT_CALL(route_window_manager, set_triggers(A<const std::vector<Trigger*>&>())).Times(1).WillOnce([&](auto) { events.push_back("route_triggers"); });
    EXPECT_CALL(route_window_manager, set_rooms(A<const std::vector<std::weak_ptr<IRoom>>&>())).Times(1).WillOnce([&](auto) { events.push_back("route_rooms"); });
    EXPECT_CALL(route_window_manager, set_route(A<IRoute*>())).Times(1).WillOnce([&](auto) { events.push_back("route_route"); });
    EXPECT_CALL(route, clear()).Times(1).WillOnce([&] { events.push_back("route_clear"); });
    EXPECT_CALL(viewer, open(NotNull())).Times(1).WillOnce([&](auto) { events.push_back("viewer"); });

    auto application = register_test_module(nullptr, nullptr, nullptr, std::move(level_loader_ptr), nullptr, nullptr, std::move(viewer_ptr), std::move(route_ptr),
        std::move(items_window_manager_ptr), std::move(triggers_window_manager_ptr), std::move(route_window_manager_ptr), std::move(rooms_window_manager_ptr));
    application->open("test_path.tr2");

    ASSERT_EQ(events.size(), 13);
    ASSERT_EQ(events.back(), "viewer");
}

TEST(Application, PropagatesSettingsWhenUpdated)
{
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, set_settings).Times(2);

    auto application = register_test_module(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, std::move(viewer_ptr));
    viewer.on_settings({});
}

TEST(Application, SavesSettingsOnShutdown)
{
    auto [settings_loader_ptr, settings_loader] = create_mock<MockSettingsLoader>();
    EXPECT_CALL(settings_loader, save_user_settings).Times(1);
    auto application = register_test_module(nullptr, std::move(settings_loader_ptr));
}

TEST(Application, FileOpenedFromCommandLine)
{
    auto startup_options = std::make_shared<MockStartupOptions>();
    ON_CALL(*startup_options, filename).WillByDefault(testing::Return("test.tr2"));
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(level_loader, load_level("test.tr2")).Times(1);
    EXPECT_CALL(viewer, open).Times(1);
    auto application = register_test_module(nullptr, nullptr, nullptr, std::move(level_loader_ptr), nullptr, nullptr, std::move(viewer_ptr),
        nullptr, nullptr, nullptr, nullptr, nullptr, startup_options);
}

TEST(Application, FileNotOpenedWhenNotSpecified)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(level_loader, load_level("test.tr2")).Times(0);
    EXPECT_CALL(viewer, open).Times(0);
    auto application = register_test_module(nullptr, nullptr, nullptr, std::move(level_loader_ptr), nullptr, nullptr, std::move(viewer_ptr));
}
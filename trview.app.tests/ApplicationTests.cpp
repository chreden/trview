#include <trview.app/Application.h>
#include <trview.app/Mocks/Elements/ILevel.h>
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
#include <trview.common/Mocks/Windows/IDialogs.h>
#include <trview.common/Mocks/IFiles.h>
#include <trlevel/Mocks/ILevelLoader.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Resources/resource.h>
#include "NullImGuiBackend.h"

using namespace trview;
using namespace trview::tests;
using namespace testing;
using namespace trview::mocks;
using namespace trlevel::mocks;
using testing::_;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"ApplicationTests") };
            std::unique_ptr<IUpdateChecker> update_checker{ std::make_unique<NiceMock<MockUpdateChecker>>() };
            std::unique_ptr<ISettingsLoader> settings_loader{ std::make_unique<NiceMock<MockSettingsLoader>>() };
            std::unique_ptr<IFileDropper> file_dropper{ std::make_unique<NiceMock<MockFileDropper>>() };
            std::unique_ptr<trlevel::ILevelLoader> level_loader{ std::make_unique<NiceMock<MockLevelLoader>>() };
            std::unique_ptr<ILevelSwitcher> level_switcher{ std::make_unique<NiceMock<MockLevelSwitcher>>() };
            std::unique_ptr<IRecentFiles> recent_files{ std::make_unique<NiceMock<MockRecentFiles>>() };
            std::unique_ptr<IViewer> viewer{ std::make_unique<NiceMock<MockViewer>>() };
            IRoute::Source route_source{ [](auto&&...) { return std::make_shared<NiceMock<MockRoute>>(); } };
            std::shared_ptr<MockShortcuts> shortcuts{ std::make_shared<NiceMock<MockShortcuts>>() };
            std::unique_ptr<IItemsWindowManager> items_window_manager{ std::make_unique<NiceMock<MockItemsWindowManager>>() };
            std::unique_ptr<ITriggersWindowManager> triggers_window_manager{ std::make_unique<NiceMock<MockTriggersWindowManager>>() };
            std::unique_ptr<IRouteWindowManager> route_window_manager{ std::make_unique<NiceMock<MockRouteWindowManager>>() };
            std::unique_ptr<IRoomsWindowManager> rooms_window_manager{ std::make_unique<NiceMock<MockRoomsWindowManager>>() };
            ILevel::Source level_source{ [](auto&&...) { return std::make_unique<NiceMock<trview::mocks::MockLevel>>(); } };
            std::shared_ptr<IStartupOptions> startup_options{ std::make_shared<NiceMock<MockStartupOptions>>() };
            std::shared_ptr<IDialogs> dialogs{ std::make_shared<NiceMock<MockDialogs>>() };
            std::shared_ptr<IFiles> files{ std::make_shared<NiceMock<MockFiles>>() };
            std::unique_ptr<IImGuiBackend> imgui_backend{ std::make_unique<NullImGuiBackend>() };

            std::unique_ptr<Application> build()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
                return std::make_unique<Application>(window, std::move(update_checker), std::move(settings_loader), std::move(file_dropper),
                    std::move(level_loader), std::move(level_switcher), std::move(recent_files), std::move(viewer), route_source, shortcuts,
                    std::move(items_window_manager), std::move(triggers_window_manager), std::move(route_window_manager), std::move(rooms_window_manager),
                    level_source, startup_options, dialogs, files, std::move(imgui_backend));
            }

            test_module& with_dialogs(std::shared_ptr<IDialogs> dialogs)
            {
                this->dialogs = dialogs;
                return *this;
            }

            test_module& with_file_dropper(std::unique_ptr<IFileDropper> file_dropper)
            {
                this->file_dropper = std::move(file_dropper);
                return *this;
            }

            test_module& with_items_window_manager(std::unique_ptr<IItemsWindowManager> items_window_manager)
            {
                this->items_window_manager = std::move(items_window_manager);
                return *this;
            }

            test_module& with_level_loader(std::unique_ptr<trlevel::ILevelLoader> level_loader)
            {
                this->level_loader = std::move(level_loader);
                return *this;
            }

            test_module& with_level_switcher(std::unique_ptr<ILevelSwitcher> level_switcher)
            {
                this->level_switcher = std::move(level_switcher);
                return *this;
            }

            test_module& with_recent_files(std::unique_ptr<IRecentFiles> recent_files)
            {
                this->recent_files = std::move(recent_files);
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

TEST(Application, FileDropperOpensFile)
{
    auto [file_dropper_ptr, file_dropper] = create_mock<MockFileDropper>();
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    EXPECT_CALL(level_loader, load_level("test_path.tr2")).Times(1).WillRepeatedly(Throw(std::exception()));
    auto application = register_test_module().with_file_dropper(std::move(file_dropper_ptr)).with_level_loader(std::move(level_loader_ptr)).build();
    file_dropper.on_file_dropped("test_path.tr2");
}

TEST(Application, LevelLoadedOnSwitchLevel)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [level_switcher_ptr, level_switcher] = create_mock<MockLevelSwitcher>();
    EXPECT_CALL(level_loader, load_level("test_path.tr2")).Times(1).WillRepeatedly(Throw(std::exception()));
    auto application = register_test_module().with_level_loader(std::move(level_loader_ptr)).with_level_switcher(std::move(level_switcher_ptr)).build();
    level_switcher.on_switch_level("test_path.tr2");
}

TEST(Application, LevelLoadedOnRecentFileOpen)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [recent_files_ptr, recent_files] = create_mock<MockRecentFiles>();
    EXPECT_CALL(level_loader, load_level("test_path.tr2")).Times(1).WillRepeatedly(Throw(std::exception()));
    auto application = register_test_module().with_level_loader(std::move(level_loader_ptr)).with_recent_files(std::move(recent_files_ptr)).build();
    recent_files.on_file_open("test_path.tr2");
}

TEST(Application, RecentFilesUpdatedOnFileOpen)
{
    auto [recent_files_ptr, recent_files] = create_mock<MockRecentFiles>();
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    EXPECT_CALL(recent_files, set_recent_files(std::list<std::string>{})).Times(1);
    EXPECT_CALL(recent_files, set_recent_files(std::list<std::string>{"test_path.tr2"})).Times(1);
    EXPECT_CALL(level_loader, load_level("test_path.tr2")).WillOnce(Return(ByMove(std::make_unique<trlevel::mocks::MockLevel>())));
    auto application = register_test_module().with_level_loader(std::move(level_loader_ptr)).with_recent_files(std::move(recent_files_ptr)).build();
    application->open("test_path.tr2");
}

TEST(Application, FileOpenedInViewer)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    auto [level_ptr, level] = create_mock<trlevel::mocks::MockLevel>();
    EXPECT_CALL(level_loader, load_level("test_path.tr2")).WillOnce(Return(ByMove(std::move(level_ptr))));
    EXPECT_CALL(viewer, open(NotNull())).Times(1);
    auto application = register_test_module().with_level_loader(std::move(level_loader_ptr)).with_viewer(std::move(viewer_ptr)).build();
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
    auto route = mock_shared<MockRoute>();

    std::vector<std::string> events;

    EXPECT_CALL(level_loader, load_level("test_path.tr2")).WillOnce(Return(ByMove(std::move(level_ptr))));
    
    EXPECT_CALL(items_window_manager, set_items(A<const std::vector<Item>&>())).Times(1).WillOnce([&](auto) { events.push_back("items_items"); });
    EXPECT_CALL(items_window_manager, set_triggers(A<const std::vector<std::weak_ptr<ITrigger>>&>())).Times(1).WillOnce([&](auto) { events.push_back("items_triggers"); });
    EXPECT_CALL(triggers_window_manager, set_items(A<const std::vector<Item>&>())).Times(1).WillOnce([&](auto) { events.push_back("triggers_items"); });
    EXPECT_CALL(triggers_window_manager, set_triggers(A<const std::vector<std::weak_ptr<ITrigger>>&>())).Times(1).WillOnce([&](auto) { events.push_back("triggers_triggers"); });
    EXPECT_CALL(rooms_window_manager, set_level_version(A<trlevel::LevelVersion>())).Times(1).WillOnce([&](auto) { events.push_back("rooms_version"); });
    EXPECT_CALL(rooms_window_manager, set_items(A<const std::vector<Item>&>())).Times(1).WillOnce([&](auto) { events.push_back("rooms_items"); });
    EXPECT_CALL(rooms_window_manager, set_triggers(A<const std::vector<std::weak_ptr<ITrigger>>&>())).Times(1).WillOnce([&](auto) { events.push_back("rooms_triggers"); });
    EXPECT_CALL(rooms_window_manager, set_rooms(A<const std::vector<std::weak_ptr<IRoom>>&>())).Times(1).WillOnce([&](auto) { events.push_back("rooms_rooms"); });
    EXPECT_CALL(route_window_manager, set_items(A<const std::vector<Item>&>())).Times(1).WillOnce([&](auto) { events.push_back("route_items"); });
    EXPECT_CALL(route_window_manager, set_triggers(A<const std::vector<std::weak_ptr<ITrigger>>&>())).Times(1).WillOnce([&](auto) { events.push_back("route_triggers"); });
    EXPECT_CALL(route_window_manager, set_rooms(A<const std::vector<std::weak_ptr<IRoom>>&>())).Times(1).WillOnce([&](auto) { events.push_back("route_rooms"); });
    EXPECT_CALL(route_window_manager, set_route(A<IRoute*>())).Times(1).WillOnce([&](auto) { events.push_back("route_route"); });
    EXPECT_CALL(*route, clear()).Times(1).WillOnce([&] { events.push_back("route_clear"); });
    EXPECT_CALL(*route, set_unsaved(false)).Times(1);
    EXPECT_CALL(viewer, open(NotNull())).Times(1).WillOnce([&](auto) { events.push_back("viewer"); });

    auto application = register_test_module()
        .with_level_loader(std::move(level_loader_ptr))
        .with_viewer(std::move(viewer_ptr))
        .with_route_source([&](auto&&...) {return route; })
        .with_items_window_manager(std::move(items_window_manager_ptr))
        .with_triggers_window_manager(std::move(triggers_window_manager_ptr))
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .build();
    application->open("test_path.tr2");

    ASSERT_EQ(events.size(), 14);
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
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(level_loader, load_level("test.tr2")).Times(1);
    EXPECT_CALL(viewer, open).Times(1);
    auto application = register_test_module().with_level_loader(std::move(level_loader_ptr)).with_viewer(std::move(viewer_ptr)).with_startup_options(startup_options).build();
}

TEST(Application, FileNotOpenedWhenNotSpecified)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(level_loader, load_level("test.tr2")).Times(0);
    EXPECT_CALL(viewer, open).Times(0);
    auto application = register_test_module().with_level_loader(std::move(level_loader_ptr)).with_viewer(std::move(viewer_ptr)).build();
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
    auto [level_loader_ptr, level_loader] = create_mock<trlevel::mocks::MockLevelLoader>();

    EXPECT_CALL(route, is_unsaved).WillRepeatedly(Return(true));
    EXPECT_CALL(level_loader, load_level).Times(0);
    auto application = register_test_module()
        .with_route_source([&](auto&&...) {return std::move(route_ptr_actual); })
        .with_dialogs(dialogs)
        .with_level_loader(std::move(level_loader_ptr))
        .build();
    application->open("");
}

TEST(Application, DialogShownOnOpenWithUnsavedRouteAllowsOpen)
{
    auto [route_ptr, route] = create_mock<MockRoute>();
    auto dialogs = mock_shared<MockDialogs>();
    auto route_ptr_actual = std::move(route_ptr);
    auto [level_loader_ptr, level_loader] = create_mock<trlevel::mocks::MockLevelLoader>();

    EXPECT_CALL(route, is_unsaved).WillRepeatedly(Return(true));
    EXPECT_CALL(*dialogs, message_box).WillRepeatedly(Return(true));
    EXPECT_CALL(level_loader, load_level).Times(1);
    auto application = register_test_module()
        .with_route_source([&](auto&&...) {return std::move(route_ptr_actual); })
        .with_dialogs(dialogs)
        .with_level_loader(std::move(level_loader_ptr))
        .build();
    application->open("");
}

TEST(Application, ClosingEventCalled)
{
    auto application = register_test_module().build();
    bool closing_called = false;
    auto token = application->on_closing += [&]() { closing_called = true; };
    application->process_message(WM_CLOSE, 0, 0);
    ASSERT_TRUE(closing_called);
}

TEST(Application, FileOpenOpensFile)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    EXPECT_CALL(level_loader, load_level).Times(1);
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename" }));

    auto application = register_test_module().with_level_loader(std::move(level_loader_ptr)).with_dialogs(dialogs).build();
    application->process_message(WM_COMMAND, MAKEWPARAM(ID_FILE_OPEN, 0), 0);
}

TEST(Application, FileOpenDoesNotOpenFileWhenCancelled)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    EXPECT_CALL(level_loader, load_level).Times(0);
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1);

    auto application = register_test_module().with_level_loader(std::move(level_loader_ptr)).with_dialogs(dialogs).build();
    application->process_message(WM_COMMAND, MAKEWPARAM(ID_FILE_OPEN, 0), 0);
}

TEST(Application, FileOpenAcceleratorOpensFile)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    EXPECT_CALL(level_loader, load_level).Times(1);
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1).WillRepeatedly(Return(IDialogs::FileResult{ "filename" }));

    auto application = register_test_module().with_level_loader(std::move(level_loader_ptr)).with_dialogs(dialogs).build();
    application->process_message(WM_COMMAND, MAKEWPARAM(ID_ACCEL_FILE_OPEN, 0), 0);
}

TEST(Application, FileOpenAcceleratorDoesNotOpenFileWhenCancelled)
{
    auto [level_loader_ptr, level_loader] = create_mock<MockLevelLoader>();
    EXPECT_CALL(level_loader, load_level).Times(0);
    auto dialogs = mock_shared<MockDialogs>();
    EXPECT_CALL(*dialogs, open_file).Times(1);

    auto application = register_test_module().with_level_loader(std::move(level_loader_ptr)).with_dialogs(dialogs).build();
    application->process_message(WM_COMMAND, MAKEWPARAM(ID_ACCEL_FILE_OPEN, 0), 0);
}

TEST(Application, ExportRouteSavesFile)
{
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    auto files = mock_shared<MockFiles>();
    EXPECT_CALL(*files, save_file(An<const std::string&>(), An<const std::string&>())).Times(1);
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, set_unsaved(false)).Times(1);

    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_route_source([&](auto&&...) {return route; })
        .with_files(files).build();

    route_window_manager.on_route_export("filename", false);
}

TEST(Application, ImportRouteLoadsFile)
{
    auto [viewer_ptr, viewer] = create_mock <MockViewer>();
    EXPECT_CALL(viewer, set_route).Times(1);
    auto [route_window_manager_ptr, route_window_manager] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route_window_manager, set_route).Times(1);
    auto files = mock_shared<MockFiles>();
    EXPECT_CALL(*files, load_file).Times(1).WillRepeatedly(Return<std::vector<uint8_t>>({ 0x7b, 0x7d }));;
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, set_unsaved(false)).Times(1);

    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_route_source([&](auto&&...) {return route; })
        .with_viewer(std::move(viewer_ptr))
        .with_files(files).build();

    route_window_manager.on_route_import("filename", false);
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

    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_items_window_manager(std::move(items_window_manager_ptr))
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .with_triggers_window_manager(std::move(triggers_window_manager_ptr))
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
    auto [viewer_ptr, viewer] = create_mock<MockViewer>();
    EXPECT_CALL(viewer, render).Times(1);

    auto application = register_test_module()
        .with_route_window_manager(std::move(route_window_manager_ptr))
        .with_items_window_manager(std::move(items_window_manager_ptr))
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .with_triggers_window_manager(std::move(triggers_window_manager_ptr))
        .with_viewer(std::move(viewer_ptr))
        .build();
    application->render();
}

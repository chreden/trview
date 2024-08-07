#include <trview.app/Windows/Windows.h>

#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.app/Mocks/Elements/IItem.h>
#include <trview.app/Mocks/Elements/ILight.h>
#include <trview.app/Mocks/Elements/ITrigger.h>

#include <trview.app/Mocks/Windows/ICameraSinkWindowManager.h>
#include <trview.app/Mocks/Windows/IConsoleManager.h>
#include <trview.app/Mocks/Windows/IItemsWindowManager.h>
#include <trview.app/Mocks/Windows/ILightsWindowManager.h>
#include <trview.app/Mocks/Windows/ILogWindowManager.h>
#include <trview.app/Mocks/Windows/IRoomsWindowManager.h>
#include <trview.app/Mocks/Windows/IRouteWindowManager.h>
#include <trview.app/Mocks/Windows/IPluginsWindowManager.h>
#include <trview.app/Mocks/Windows/IStaticsWindowManager.h>
#include <trview.app/Mocks/Windows/ITexturesWindowManager.h>
#include <trview.app/Mocks/Windows/ITriggersWindowManager.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::unique_ptr<ICameraSinkWindowManager> camera_sinks{ mock_unique<MockCameraSinkWindowManager>() };
            std::unique_ptr<IConsoleManager> console_manager{ mock_unique<MockConsoleManager>() };
            std::unique_ptr<IItemsWindowManager> items{ mock_unique<MockItemsWindowManager>() };
            std::unique_ptr<ILogWindowManager> log{ mock_unique<MockLogWindowManager>() };
            std::unique_ptr<ILightsWindowManager> lights{ mock_unique<MockLightsWindowManager>() };
            std::unique_ptr<IPluginsWindowManager> plugins{ mock_unique<MockPluginsWindowManager>() };
            std::unique_ptr<IRoomsWindowManager> rooms{ mock_unique<MockRoomsWindowManager>() };
            std::unique_ptr<IRouteWindowManager> route{ mock_unique<MockRouteWindowManager>() };
            std::unique_ptr<IStaticsWindowManager> statics{ mock_unique<MockStaticsWindowManager>() };
            std::unique_ptr<ITexturesWindowManager> textures{ mock_unique<MockTexturesWindowManager>() };
            std::unique_ptr<ITriggersWindowManager> triggers{ mock_unique<MockTriggersWindowManager>() };

            std::unique_ptr<trview::Windows> build()
            {
                return std::make_unique<trview::Windows>(
                    std::move(camera_sinks),
                    std::move(console_manager),
                    std::move(items),
                    std::move(lights),
                    std::move(log),
                    std::move(plugins),
                    std::move(rooms),
                    std::move(route),
                    std::move(statics),
                    std::move(textures),
                    std::move(triggers));
            }

            test_module& with_camera_sinks(std::unique_ptr<ICameraSinkWindowManager> manager)
            {
                camera_sinks = std::move(manager);
                return *this;
            }

            test_module& with_items(std::unique_ptr<IItemsWindowManager> manager)
            {
                items = std::move(manager);
                return *this;
            }

            test_module& with_lights(std::unique_ptr<ILightsWindowManager> manager)
            {
                lights = std::move(manager);
                return *this;
            }

            test_module& with_rooms(std::unique_ptr<IRoomsWindowManager> manager)
            {
                rooms = std::move(manager);
                return *this;
            }
        };
        return test_module{};
    }
}

TEST(Windows, CameraSinkEventsForwarded)
{
    auto [camera_sinks_ptr, camera_sinks] = create_mock<MockCameraSinkWindowManager>();
    auto windows = register_test_module().with_camera_sinks(std::move(camera_sinks_ptr)).build();

    std::shared_ptr<ICameraSink> raised_camera;
    auto t1 = windows->on_camera_sink_selected += [&](auto s) { raised_camera = s.lock(); };

    std::shared_ptr<ITrigger> raised_trigger;
    auto t2 = windows->on_trigger_selected += [&](auto t) { raised_trigger = t.lock(); };

    bool raised = false;
    auto t3 = windows->on_scene_changed += [&]() { raised = true; };

    auto camera = mock_shared<MockCameraSink>();
    auto trigger = mock_shared<MockTrigger>();

    camera_sinks.on_camera_sink_selected(camera);
    camera_sinks.on_trigger_selected(trigger);
    camera_sinks.on_scene_changed();

    ASSERT_EQ(raised_camera, camera);
    ASSERT_EQ(raised_trigger, trigger);
    ASSERT_EQ(raised, true);
}

TEST(Windows, ItemsEventsForwarded)
{
    auto [items_ptr, items] = create_mock<MockItemsWindowManager>();
    auto windows = register_test_module().with_items(std::move(items_ptr)).build();

    std::shared_ptr<IItem> raised_item;
    auto t1 = windows->on_item_selected += [&](auto s) { raised_item = s.lock(); };

    std::shared_ptr<ITrigger> raised_trigger;
    auto t2 = windows->on_trigger_selected += [&](auto t) { raised_trigger = t.lock(); };

    bool raised = false;
    auto t3 = windows->on_scene_changed += [&]() { raised = true; };

    auto item = mock_shared<MockItem>();
    auto trigger = mock_shared<MockTrigger>();

    items.on_item_selected(item);
    items.on_trigger_selected(trigger);
    items.on_scene_changed();

    ASSERT_EQ(raised_item, item);
    ASSERT_EQ(raised_trigger, trigger);
    ASSERT_EQ(raised, true);
}

TEST(Windows, LightsEventsForwarded)
{
    auto [lights_ptr, lights] = create_mock<MockLightsWindowManager>();
    auto windows = register_test_module().with_lights(std::move(lights_ptr)).build();

    std::shared_ptr<ILight> raised_light;
    auto t1 = windows->on_light_selected += [&](auto s) { raised_light = s.lock(); };

    bool raised = false;
    auto t2 = windows->on_scene_changed += [&]() { raised = true; };

    auto light = mock_shared<MockLight>();
    auto trigger = mock_shared<MockTrigger>();

    lights.on_light_selected(light);
    lights.on_scene_changed();

    ASSERT_EQ(raised_light, light);
    ASSERT_EQ(raised, true);
}

TEST(Windows, RoomsEventsForwarded)
{
    FAIL();
}

TEST(Windows, RouteEventsForwarded)
{
    FAIL();
}

TEST(Windows, SelectCameraSink)
{
    auto [camera_sinks_ptr, camera_sinks] = create_mock<MockCameraSinkWindowManager>();
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();

    const auto camera = mock_shared<MockCameraSink>();
    std::shared_ptr<ICameraSink> camera_sinks_camera;
    EXPECT_CALL(camera_sinks, set_selected_camera_sink).Times(1).WillRepeatedly([&](auto c) { camera_sinks_camera = c.lock(); });
    std::shared_ptr<ICameraSink> rooms_camera;
    EXPECT_CALL(rooms, set_selected_camera_sink).Times(1).WillRepeatedly([&](auto c) { rooms_camera = c.lock(); });

    auto windows = register_test_module().with_camera_sinks(std::move(camera_sinks_ptr)).with_rooms(std::move(rooms_ptr)).build();

    windows->select(camera);

    ASSERT_EQ(camera_sinks_camera, camera);
    ASSERT_EQ(rooms_camera, camera);
}

TEST(Windows, SelectItem)
{
    FAIL();
}

TEST(Windows, SelectLight)
{
    FAIL();
}

TEST(Windows, SelectStaticMesh)
{
    FAIL();
}

TEST(Windows, SelectTrigger)
{
    FAIL();
}

TEST(Windows, SelectWaypoint)
{
    FAIL();
}

TEST(Windows, SetLevel)
{
    FAIL();
}

TEST(Windows, SetRoom)
{
    FAIL();
}

TEST(Windows, SetRoute)
{
    FAIL();
}

TEST(Windows, SetSettings)
{
    FAIL();
}

TEST(Windows, Setup)
{
    FAIL();
}

TEST(Windows, StaticsEventsForwarded)
{
    FAIL();
}

TEST(Windows, TriggersEventsForwarded)
{
    FAIL();
}

TEST(Windows, WindowsRendered)
{
    FAIL();
}

TEST(Windows, WindowsUpdated)
{
    FAIL();
}

/*

TEST(Windows, MapColoursSetOnRoomWindow)
{
    auto [rooms_window_manager_ptr, rooms_window_manager] = create_mock<MockRoomsWindowManager>();
    EXPECT_CALL(rooms_window_manager, set_map_colours).Times(1);

    auto windows = register_test_module()
        .with_rooms_window_manager(std::move(rooms_window_manager_ptr))
        .build();
}

TEST(Windows, MapColoursSetOnSettingsChanged)
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

TEST(Windows, SetLevel)
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
    auto [windows_ptr, windows] = create_mock<MockWindows>();
    auto route = mock_shared<MockRoute>();

    std::vector<std::string> events;

    EXPECT_CALL(items_window_manager, set_items(A<const std::vector<std::weak_ptr<IItem>>&>())).Times(1).WillOnce([&](auto) { events.push_back("items_items"); });
    EXPECT_CALL(items_window_manager, set_triggers(A<const std::vector<std::weak_ptr<ITrigger>>&>())).Times(1).WillOnce([&](auto) { events.push_back("items_triggers"); });
    EXPECT_CALL(items_window_manager, set_level_version(A<trlevel::LevelVersion>())).Times(1).WillOnce([&](auto) { events.push_back("items_version"); });
    EXPECT_CALL(items_window_manager, set_model_checker(A<const std::function<bool(uint32_t)>&>())).Times(1).WillOnce([&](auto) { events.push_back("items_model_checker"); });
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
        .with_windows(std::move(windows_ptr))
        .build();
    application->open("test_path.tr2", ILevel::OpenMode::Full);

    ASSERT_TRUE(called.has_value());
    ASSERT_EQ(called.value(), "test_path.tr2");

    ASSERT_EQ(events.back(), "viewer");
}

TEST(Windows, WindowsRendered)
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
    auto [console_manager_ptr, console_manager] = create_mock<MockConsoleManager>();
    EXPECT_CALL(console_manager, render).Times(1);
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
        .with_console_manager(std::move(console_manager_ptr))
        .with_windows(std::move(windows_ptr))
        .with_viewer(std::move(viewer_ptr))
        .with_plugins(plugins)
        .build();
    application->render();
}

TEST(Windows, WindowsUpdated)
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

TEST(Windows, RouteWindowCreatedOnStartup)
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

TEST(Windows, RouteWindowNotCreatedOnStartup)
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
*/
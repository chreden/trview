#include <trview.app/Windows/Windows.h>

#include <trview.common/TokenStore.h>

#include <trview.app/Mocks/Elements/ICameraSink.h>
#include <trview.app/Mocks/Elements/IItem.h>
#include <trview.app/Mocks/Elements/ILight.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.app/Mocks/Elements/ISoundSource.h>
#include <trview.app/Mocks/Elements/ITrigger.h>

#include <trview.app/Mocks/Windows/IAboutWindowManager.h>
#include <trview.app/Mocks/Windows/ICameraSinkWindowManager.h>
#include <trview.app/Mocks/Windows/IConsoleManager.h>
#include <trview.app/Mocks/Windows/IDiffWindowManager.h>
#include <trview.app/Mocks/Windows/IItemsWindowManager.h>
#include <trview.app/Mocks/Windows/ILightsWindowManager.h>
#include <trview.app/Mocks/Windows/ILogWindowManager.h>
#include <trview.app/Mocks/Windows/IRoomsWindowManager.h>
#include <trview.app/Mocks/Windows/IRouteWindowManager.h>
#include <trview.app/Mocks/Windows/IPluginsWindowManager.h>
#include <trview.app/Mocks/Windows/ISoundsWindowManager.h>
#include <trview.app/Mocks/Windows/IStaticsWindowManager.h>
#include <trview.app/Mocks/Windows/ITexturesWindowManager.h>
#include <trview.app/Mocks/Windows/ITriggersWindowManager.h>
#include <trview.app/Mocks/Windows/IPackWindowManager.h>

#include <trview.tests.common/Event.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using namespace DirectX::SimpleMath;
using testing::A;
using testing::Return;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::unique_ptr<IAboutWindowManager> about_window{ mock_unique<MockAboutWindowManager>() };
            std::unique_ptr<ICameraSinkWindowManager> camera_sinks{ mock_unique<MockCameraSinkWindowManager>() };
            std::unique_ptr<IConsoleManager> console_manager{ mock_unique<MockConsoleManager>() };
            std::unique_ptr<IDiffWindowManager> diffs{ mock_unique<MockDiffWindowManager>() };
            std::shared_ptr<IItemsWindowManager> items{ mock_shared<MockItemsWindowManager>() };
            std::unique_ptr<ILogWindowManager> log{ mock_unique<MockLogWindowManager>() };
            std::unique_ptr<ILightsWindowManager> lights{ mock_unique<MockLightsWindowManager>() };
            std::unique_ptr<IPackWindowManager> pack{ mock_unique<MockPackWindowManager>() };
            std::unique_ptr<IPluginsWindowManager> plugins{ mock_unique<MockPluginsWindowManager>() };
            std::unique_ptr<IRoomsWindowManager> rooms{ mock_unique<MockRoomsWindowManager>() };
            std::unique_ptr<IRouteWindowManager> route{ mock_unique<MockRouteWindowManager>() };
            std::unique_ptr<ISoundsWindowManager> sounds{ mock_unique<MockSoundsWindowManager>() };
            std::unique_ptr<IStaticsWindowManager> statics{ mock_unique<MockStaticsWindowManager>() };
            std::unique_ptr<ITexturesWindowManager> textures{ mock_unique<MockTexturesWindowManager>() };
            std::unique_ptr<ITriggersWindowManager> triggers{ mock_unique<MockTriggersWindowManager>() };

            std::unique_ptr<trview::Windows> build()
            {
                return std::make_unique<trview::Windows>(
                    std::move(about_window),
                    std::move(camera_sinks),
                    std::move(console_manager),
                    std::move(diffs),
                    items,
                    std::move(lights),
                    std::move(log),
                    std::move(pack),
                    std::move(plugins),
                    std::move(rooms),
                    std::move(route),
                    std::move(sounds),
                    std::move(statics),
                    std::move(textures),
                    std::move(triggers));
            }

            test_module& with_camera_sinks(std::unique_ptr<ICameraSinkWindowManager> manager)
            {
                camera_sinks = std::move(manager);
                return *this;
            }

            test_module& with_console(std::unique_ptr<IConsoleManager> manager)
            {
                console_manager = std::move(manager);
                return *this;
            }

            test_module& with_diffs(std::unique_ptr<IDiffWindowManager> manager)
            {
                diffs = std::move(manager);
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

            test_module& with_log(std::unique_ptr<ILogWindowManager> manager)
            {
                log = std::move(manager);
                return *this;
            }

            test_module& with_packs(std::unique_ptr<IPackWindowManager> manager)
            {
                pack = std::move(manager);
                return *this;
            }

            test_module& with_plugins(std::unique_ptr<IPluginsWindowManager> manager)
            {
                plugins = std::move(manager);
                return *this;
            }

            test_module& with_rooms(std::unique_ptr<IRoomsWindowManager> manager)
            {
                rooms = std::move(manager);
                return *this;
            }

            test_module& with_route(std::unique_ptr<IRouteWindowManager> manager)
            {
                route = std::move(manager);
                return *this;
            }

            test_module& with_sounds(std::unique_ptr<ISoundsWindowManager> manager)
            {
                sounds = std::move(manager);
                return *this;
            }

            test_module& with_statics(std::unique_ptr<IStaticsWindowManager> manager)
            {
                statics = std::move(manager);
                return *this;
            }

            test_module& with_textures(std::unique_ptr<ITexturesWindowManager> manager)
            {
                textures = std::move(manager);
                return *this;
            }

            test_module& with_triggers(std::unique_ptr<ITriggersWindowManager> manager)
            {
                triggers = std::move(manager);
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

    TokenStore store;
    std::shared_ptr<ICameraSink> raised_camera;
    store += windows->on_camera_sink_selected += capture(raised_camera);

    std::shared_ptr<ITrigger> raised_trigger;
    store += windows->on_trigger_selected += capture(raised_trigger);

    bool raised = false;
    store += windows->on_scene_changed += capture_called(raised);

    auto camera = mock_shared<MockCameraSink>();
    auto trigger = mock_shared<MockTrigger>();

    camera_sinks.on_camera_sink_selected(camera);
    camera_sinks.on_trigger_selected(trigger);
    camera_sinks.on_scene_changed();

    ASSERT_EQ(raised_camera, camera);
    ASSERT_EQ(raised_trigger, trigger);
    ASSERT_EQ(raised, true);
}

TEST(Windows, CameraSinkStartup)
{
    auto [camera_sinks_ptr, camera_sinks] = create_mock<MockCameraSinkWindowManager>();
    EXPECT_CALL(camera_sinks, create_window).Times(1);

    auto windows = register_test_module().with_camera_sinks(std::move(camera_sinks_ptr)).build();

    windows->setup({ .camera_sink_startup = true });
}

TEST(Windows, ItemsEventsForwarded)
{
    auto [items_ptr, items] = create_mock<MockItemsWindowManager>();
    auto waypoint = mock_shared<MockWaypoint>();
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, insert(A<const Vector3&>(), A<const Vector3&>(), A<uint32_t>(), IWaypoint::Type::Entity, A<uint32_t>())).Times(1).WillRepeatedly(Return(100));
    EXPECT_CALL(*route, waypoint(100)).Times(1).WillRepeatedly(Return(waypoint));

    auto windows = register_test_module().with_items(std::move(items_ptr)).build();
    windows->set_route(route);

    TokenStore store;
    std::shared_ptr<IWaypoint> raised_waypoint;
    store += windows->on_waypoint_selected += capture(raised_waypoint);

    std::shared_ptr<IItem> raised_item;
    store += windows->on_item_selected += capture(raised_item);

    std::shared_ptr<ITrigger> raised_trigger;
    store += windows->on_trigger_selected += capture(raised_trigger);

    bool raised = false;
    store += windows->on_scene_changed += capture_called(raised);

    auto item = mock_shared<MockItem>();
    auto trigger = mock_shared<MockTrigger>();

    items.on_add_to_route(item);
    items.on_item_selected(item);
    items.on_trigger_selected(trigger);
    items.on_scene_changed();

    ASSERT_EQ(raised_waypoint, waypoint);
    ASSERT_EQ(raised_item, item);
    ASSERT_EQ(raised_trigger, trigger);
    ASSERT_EQ(raised, true);
}

TEST(Windows, ItemsStartup)
{
    auto [items_ptr, items] = create_mock<MockItemsWindowManager>();
    EXPECT_CALL(items, create_window).Times(1);

    auto windows = register_test_module().with_items(std::move(items_ptr)).build();

    windows->setup({ .items_startup = true });
}

TEST(Windows, LightsEventsForwarded)
{
    auto [lights_ptr, lights] = create_mock<MockLightsWindowManager>();
    auto windows = register_test_module().with_lights(std::move(lights_ptr)).build();

    TokenStore store;
    std::shared_ptr<ILight> raised_light;
    store += windows->on_light_selected += [&](auto s) { raised_light = s.lock(); };

    bool raised = false;
    store += windows->on_scene_changed += [&]() { raised = true; };

    auto light = mock_shared<MockLight>();
    auto trigger = mock_shared<MockTrigger>();

    lights.on_light_selected(light);
    lights.on_scene_changed();

    ASSERT_EQ(raised_light, light);
    ASSERT_EQ(raised, true);
}

TEST(Windows, OnNgPlusForwarded)
{
    auto [items_ptr, items] = create_mock<MockItemsWindowManager>();
    EXPECT_CALL(items, set_ng_plus(true)).Times(1);
    EXPECT_CALL(items, set_ng_plus(false)).Times(1);
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();
    EXPECT_CALL(rooms, set_ng_plus(true)).Times(1);
    EXPECT_CALL(rooms, set_ng_plus(false)).Times(1);
    auto windows = register_test_module().with_rooms(std::move(rooms_ptr)).with_items(std::move(items_ptr)).build();

    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, ng_plus).WillByDefault(Return(true));

    windows->set_level(level);

    level->on_ng_plus(false);
}

TEST(Windows, PackEventsForwarded)
{
    auto [packs_ptr, packs] = create_mock<MockPackWindowManager>();
    auto windows = register_test_module().with_packs(std::move(packs_ptr)).build();

    TokenStore store;
    std::string raised_level;
    store += windows->on_level_open += capture(raised_level);

    packs.on_level_open("test");

    ASSERT_EQ(raised_level, "test");
}

TEST(Windows, RoomsEventsForwarded)
{
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();

    auto windows = register_test_module().with_rooms(std::move(rooms_ptr)).build();

    TokenStore store;
    std::shared_ptr<ICameraSink> raised_camera;
    store += windows->on_camera_sink_selected += capture(raised_camera);
    std::shared_ptr<IItem> raised_item;
    store += windows->on_item_selected += capture(raised_item);
    std::shared_ptr<ILight> raised_light;
    store += windows->on_light_selected += capture(raised_light);
    std::shared_ptr<IRoom> raised_room;
    store += windows->on_room_selected += capture(raised_room);
    bool scene_changed = false;
    store += windows->on_scene_changed += capture_called(scene_changed);
    std::shared_ptr<ISector> raised_sector;
    store += windows->on_sector_hover += capture(raised_sector);
    std::shared_ptr<IStaticMesh> raised_static;
    store += windows->on_static_selected += capture(raised_static);
    std::shared_ptr<ITrigger> raised_trigger;
    store += windows->on_trigger_selected += capture(raised_trigger);

    auto camera = mock_shared<MockCameraSink>();
    rooms.on_camera_sink_selected(camera);

    auto item = mock_shared<MockItem>();
    rooms.on_item_selected(item);

    auto light = mock_shared<MockLight>();
    rooms.on_light_selected(light);

    auto room = mock_shared<MockRoom>();
    rooms.on_room_selected(room);

    rooms.on_scene_changed();

    auto sector = mock_shared<MockSector>();
    rooms.on_sector_hover(sector);

    auto static_mesh = mock_shared<MockStaticMesh>();
    rooms.on_static_mesh_selected(static_mesh);

    auto trigger = mock_shared<MockTrigger>();
    rooms.on_trigger_selected(trigger);

    ASSERT_EQ(raised_camera, camera);
    ASSERT_EQ(raised_item, item);
    ASSERT_EQ(raised_light, light);
    ASSERT_EQ(raised_room, room);
    ASSERT_EQ(scene_changed, true);
    ASSERT_EQ(raised_sector, sector);
    ASSERT_EQ(raised_static, static_mesh);
    ASSERT_EQ(raised_trigger, trigger);
}

TEST(Windows, RoomsStartup)
{
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();
    EXPECT_CALL(rooms, create_window).Times(1);

    auto windows = register_test_module().with_rooms(std::move(rooms_ptr)).build();

    windows->setup({ .rooms_startup = true });
}

TEST(Windows, RouteEventsForwarded)
{
    auto [route_ptr, route] = create_mock<MockRouteWindowManager>();
    auto windows = register_test_module().with_route(std::move(route_ptr)).build();

    TokenStore store;

    std::shared_ptr<IWaypoint> raised_waypoint;
    store += route.on_waypoint_selected += capture(raised_waypoint);
    std::shared_ptr<IItem> raised_item;
    store += route.on_item_selected += capture(raised_item);
    bool scene_changed = false;
    store += route.on_scene_changed += capture_called(scene_changed);
    std::shared_ptr<ITrigger> raised_trigger;
    store += route.on_trigger_selected += capture(raised_trigger);
    bool route_open = false;
    store += route.on_route_open += capture_called(route_open);
    bool route_reload = false;
    store += route.on_route_reload += capture_called(route_reload);
    bool route_save = false;
    store += route.on_route_save += capture_called(route_save);
    bool route_save_as = false;
    store += route.on_route_save_as += capture_called(route_save_as);
    std::optional<std::string> level_switch_raised;
    store += route.on_level_switch += capture(level_switch_raised);
    bool new_route = false;
    store += route.on_new_route += capture_called(new_route);
    bool new_randomizer_route = false;
    store += route.on_new_randomizer_route += capture_called(new_randomizer_route);

    auto waypoint = mock_shared<MockWaypoint>();
    route.on_waypoint_selected(waypoint);
    auto item = mock_shared<MockItem>();
    route.on_item_selected(item);
    route.on_scene_changed();
    auto trigger = mock_shared<MockTrigger>();
    route.on_trigger_selected(trigger);
    route.on_route_open();
    route.on_route_reload();
    route.on_route_save();
    route.on_route_save_as();
    route.on_level_switch("test");
    route.on_new_route();
    route.on_new_randomizer_route();

    ASSERT_EQ(raised_waypoint, waypoint);
    ASSERT_EQ(raised_item, item);
    ASSERT_EQ(scene_changed, true);
    ASSERT_EQ(raised_trigger, trigger);
    ASSERT_EQ(route_open, true);
    ASSERT_EQ(route_reload, true);
    ASSERT_EQ(route_save, true);
    ASSERT_EQ(route_save_as, true);
    ASSERT_EQ(level_switch_raised, "test");
    ASSERT_EQ(new_route, true);
    ASSERT_EQ(new_randomizer_route, true);
}

TEST(Windows, RouteStartup)
{
    auto [route_ptr, route] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route, create_window).Times(1);

    auto windows = register_test_module().with_route(std::move(route_ptr)).build();

    windows->setup({ .route_startup = true });
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
    auto [items_ptr, items] = create_mock<MockItemsWindowManager>();
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();
    auto windows = register_test_module()
        .with_items(std::move(items_ptr))
        .with_rooms(std::move(rooms_ptr))
        .build();

    std::shared_ptr<IItem> items_item;
    EXPECT_CALL(items, set_selected_item).Times(1).WillRepeatedly([&](auto i) { items_item = i.lock(); });
    std::shared_ptr<IItem> rooms_item;
    EXPECT_CALL(rooms, set_selected_item).Times(1).WillRepeatedly([&](auto i) { rooms_item = i.lock(); });

    const auto item = mock_shared<MockItem>();
    windows->select(item);

    ASSERT_EQ(items_item, item);
    ASSERT_EQ(rooms_item, item);
}

TEST(Windows, SelectLight)
{
    auto [lights_ptr, lights] = create_mock<MockLightsWindowManager>();
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();
    auto windows = register_test_module()
        .with_lights(std::move(lights_ptr))
        .with_rooms(std::move(rooms_ptr))
        .build();

    std::shared_ptr<ILight> lights_light;
    EXPECT_CALL(lights, set_selected_light).Times(1).WillRepeatedly([&](auto l) { lights_light = l.lock(); });
    std::shared_ptr<ILight> rooms_light;
    EXPECT_CALL(rooms, set_selected_light).Times(1).WillRepeatedly([&](auto l) { rooms_light = l.lock(); });

    const auto light = mock_shared<MockLight>();
    windows->select(light);

    ASSERT_EQ(lights_light, light);
    ASSERT_EQ(rooms_light, light);
}

TEST(Windows, SelectSoundSource)
{
    auto [sounds_ptr, sounds] = create_mock<MockSoundsWindowManager>();
    auto windows = register_test_module().with_sounds(std::move(sounds_ptr)).build();

    std::shared_ptr<ISoundSource> sounds_sound;
    EXPECT_CALL(sounds, select_sound_source).Times(1).WillRepeatedly([&](auto s) { sounds_sound = s.lock(); });

    auto sound_source = mock_shared<MockSoundSource>();
    windows->select(sound_source);

    ASSERT_EQ(sound_source, sounds_sound);
}

TEST(Windows, SelectStaticMesh)
{
    auto [statics_ptr, statics] = create_mock<MockStaticsWindowManager>();
    auto windows = register_test_module().with_statics(std::move(statics_ptr)).build();

    std::shared_ptr<IStaticMesh> statics_static;
    EXPECT_CALL(statics, select_static).Times(1).WillRepeatedly([&](auto s) { statics_static = s.lock(); });

    const auto static_mesh = mock_shared<MockStaticMesh>();
    windows->select(static_mesh);

    ASSERT_EQ(statics_static, static_mesh);
}

TEST(Windows, SelectTrigger)
{
    auto [triggers_ptr, triggers] = create_mock<MockTriggersWindowManager>();
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();
    auto windows = register_test_module()
        .with_triggers(std::move(triggers_ptr))
        .with_rooms(std::move(rooms_ptr))
        .build();

    std::shared_ptr<ITrigger> triggers_trigger;
    EXPECT_CALL(triggers, set_selected_trigger).Times(1).WillRepeatedly([&](auto t) { triggers_trigger = t.lock(); });
    std::shared_ptr<ITrigger> rooms_trigger;
    EXPECT_CALL(rooms, set_selected_trigger).Times(1).WillRepeatedly([&](auto t) { rooms_trigger = t.lock(); });

    const auto trigger = mock_shared<MockTrigger>();
    windows->select(trigger);

    ASSERT_EQ(triggers_trigger, trigger);
    ASSERT_EQ(rooms_trigger, trigger);
}

TEST(Windows, SelectWaypoint)
{
    auto [route_ptr, route] = create_mock<MockRouteWindowManager>();
    auto windows = register_test_module().with_route(std::move(route_ptr)).build();

    std::shared_ptr<IWaypoint> route_waypoint;
    EXPECT_CALL(route, select_waypoint).Times(1).WillRepeatedly([&](auto w) { route_waypoint = w.lock(); });

    const auto waypoint = mock_shared<MockWaypoint>();
    windows->select(waypoint);

    ASSERT_EQ(route_waypoint, waypoint);
}

TEST(Windows, SetLevel)
{
    auto [cameras_ptr, cameras] = create_mock<MockCameraSinkWindowManager>();
    EXPECT_CALL(cameras, set_camera_sinks).Times(1);
    EXPECT_CALL(cameras, set_flybys).Times(1);
    EXPECT_CALL(cameras, set_platform_and_version).Times(1);
    auto [items_ptr, items] = create_mock<MockItemsWindowManager>();
    EXPECT_CALL(items, set_items).Times(1);
    EXPECT_CALL(items, set_triggers).Times(1);
    EXPECT_CALL(items, set_level_version).Times(1);
    EXPECT_CALL(items, set_model_checker).Times(1);
    auto [lights_ptr, lights] = create_mock<MockLightsWindowManager>();
    EXPECT_CALL(lights, set_level_version).Times(1);
    EXPECT_CALL(lights, set_lights).Times(1);
    auto [pack_ptr, pack] = create_mock<MockPackWindowManager>();
    EXPECT_CALL(pack, set_level).Times(1);
    EXPECT_CALL(pack, set_pack).Times(1);
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();
    EXPECT_CALL(rooms, set_level_version).Times(1);
    EXPECT_CALL(rooms, set_items).Times(1);
    EXPECT_CALL(rooms, set_floordata).Times(1);
    EXPECT_CALL(rooms, set_rooms).Times(1);
    EXPECT_CALL(rooms, set_ng_plus).Times(1);
    EXPECT_CALL(rooms, set_trng).Times(1);
    auto [route_ptr, route] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route, set_items).Times(1);
    EXPECT_CALL(route, set_triggers).Times(1);
    EXPECT_CALL(route, set_rooms).Times(1);
    auto [sounds_ptr, sounds] = create_mock<MockSoundsWindowManager>();
    EXPECT_CALL(sounds, set_level_version).Times(1);
    EXPECT_CALL(sounds, set_sound_sources).Times(1);
    EXPECT_CALL(sounds, set_sound_storage).Times(1);
    auto [statics_ptr, statics] = create_mock<MockStaticsWindowManager>();
    EXPECT_CALL(statics, set_statics).Times(1);
    auto [triggers_ptr, triggers] = create_mock<MockTriggersWindowManager>();
    EXPECT_CALL(triggers, set_items).Times(1);
    EXPECT_CALL(triggers, set_triggers).Times(1);
    EXPECT_CALL(triggers, set_platform_and_version).Times(1);
    auto [textures_ptr, textures] = create_mock<MockTexturesWindowManager>();
    EXPECT_CALL(textures, set_texture_storage).Times(1);
    auto windows = register_test_module()
        .with_camera_sinks(std::move(cameras_ptr))
        .with_items(std::move(items_ptr))
        .with_lights(std::move(lights_ptr))
        .with_packs(std::move(pack_ptr))
        .with_rooms(std::move(rooms_ptr))
        .with_route(std::move(route_ptr))
        .with_sounds(std::move(sounds_ptr))
        .with_statics(std::move(statics_ptr))
        .with_textures(std::move(textures_ptr))
        .with_triggers(std::move(triggers_ptr))
        .build();

    windows->set_level(mock_shared<MockLevel>());
}

TEST(Windows, SetRoom)
{
    auto [cameras_ptr, cameras] = create_mock<MockCameraSinkWindowManager>();
    auto [items_ptr, items] = create_mock<MockItemsWindowManager>();
    auto [lights_ptr, lights] = create_mock<MockLightsWindowManager>();
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();
    auto [statics_ptr, statics] = create_mock<MockStaticsWindowManager>();
    auto [triggers_ptr, triggers] = create_mock<MockTriggersWindowManager>();
    auto windows = register_test_module()
        .with_camera_sinks(std::move(cameras_ptr))
        .with_items(std::move(items_ptr))
        .with_lights(std::move(lights_ptr))
        .with_rooms(std::move(rooms_ptr))
        .with_statics(std::move(statics_ptr))
        .with_triggers(std::move(triggers_ptr))
        .build();

    std::shared_ptr<IRoom> cameras_room;
    EXPECT_CALL(cameras, set_room).Times(1).WillRepeatedly([&](auto r) { cameras_room = r.lock(); });
    std::shared_ptr<IRoom> items_room;
    EXPECT_CALL(items, set_room).Times(1).WillRepeatedly([&](auto r) { items_room = r.lock(); });
    std::shared_ptr<IRoom> lights_room;
    EXPECT_CALL(lights, set_room).Times(1).WillRepeatedly([&](auto r) { lights_room = r.lock(); });
    std::shared_ptr<IRoom> rooms_room;
    EXPECT_CALL(rooms, set_room).Times(1).WillRepeatedly([&](auto r) { rooms_room = r.lock(); });
    std::shared_ptr<IRoom> statics_room;
    EXPECT_CALL(statics, set_room).Times(1).WillRepeatedly([&](auto r) { statics_room = r.lock(); });
    std::shared_ptr<IRoom> triggers_room;
    EXPECT_CALL(triggers, set_room).Times(1).WillRepeatedly([&](auto r) { triggers_room = r.lock(); });

    const auto room = mock_shared<MockRoom>();
    windows->set_room(room);

    ASSERT_EQ(cameras_room, room);
    ASSERT_EQ(items_room, room);
    ASSERT_EQ(lights_room, room);
    ASSERT_EQ(rooms_room, room);
    ASSERT_EQ(statics_room, room);
    ASSERT_EQ(triggers_room, room);
}

TEST(Windows, SetRoute)
{
    auto [route_ptr, route] = create_mock<MockRouteWindowManager>();
    auto windows = register_test_module().with_route(std::move(route_ptr)).build();

    std::shared_ptr<IRoute> route_route;
    EXPECT_CALL(route, set_route).Times(1).WillRepeatedly([&](auto r) { route_route = r.lock(); });

    const auto real_route = mock_shared<MockRoute>();
    windows->set_route(real_route);

    ASSERT_EQ(route_route, real_route);
}

TEST(Windows, SetSettings)
{
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();
    auto [route_ptr, route] = create_mock<MockRouteWindowManager>();
    auto windows = register_test_module().with_rooms(std::move(rooms_ptr)).with_route(std::move(route_ptr)).build();

    EXPECT_CALL(rooms, set_settings).Times(1);
    EXPECT_CALL(route, set_randomizer_enabled(true)).Times(1);
    EXPECT_CALL(route, set_randomizer_settings).Times(1);

    windows->set_settings({ .randomizer_tools = true });
}

TEST(Windows, Setup)
{
    auto [cameras_ptr, cameras] = create_mock<MockCameraSinkWindowManager>();
    auto [items_ptr, items] = create_mock<MockItemsWindowManager>();
    auto [lights_ptr, lights] = create_mock<MockLightsWindowManager>();
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();
    auto [route_ptr, route] = create_mock<MockRouteWindowManager>();
    auto [statics_ptr, statics] = create_mock<MockStaticsWindowManager>();
    auto [triggers_ptr, triggers] = create_mock<MockTriggersWindowManager>();
    auto windows = register_test_module()
        .with_camera_sinks(std::move(cameras_ptr))
        .with_items(std::move(items_ptr))
        .with_lights(std::move(lights_ptr))
        .with_rooms(std::move(rooms_ptr))
        .with_route(std::move(route_ptr))
        .with_statics(std::move(statics_ptr))
        .with_triggers(std::move(triggers_ptr))
        .build();

    EXPECT_CALL(cameras, create_window).Times(0);
    EXPECT_CALL(items, create_window).Times(0);
    EXPECT_CALL(lights, create_window).Times(0);
    EXPECT_CALL(rooms, create_window).Times(0);
    EXPECT_CALL(route, create_window).Times(0);
    EXPECT_CALL(statics, create_window).Times(0);
    EXPECT_CALL(triggers, create_window).Times(0);

    EXPECT_CALL(rooms, set_settings).Times(1);
    EXPECT_CALL(route, set_randomizer_enabled(true)).Times(1);
    EXPECT_CALL(route, set_randomizer_settings).Times(1);

    windows->setup({ .randomizer_tools = true });
}

TEST(Windows, SoundsEventsForwarded)
{
    auto [sounds_ptr, sounds] = create_mock<MockSoundsWindowManager>();
    auto windows = register_test_module().with_sounds(std::move(sounds_ptr)).build();

    std::shared_ptr<ISoundSource> raised_sound_source;
    auto t1 = windows->on_sound_source_selected += capture(raised_sound_source);

    bool raised = false;
    auto t2 = windows->on_scene_changed += capture_called(raised);

    auto sound_source = mock_shared<MockSoundSource>();
    sounds.on_sound_source_selected(sound_source);

    sounds.on_scene_changed();

    ASSERT_EQ(raised_sound_source, sound_source);
    ASSERT_EQ(raised, true);
}

TEST(Windows, StaticsEventsForwarded)
{
    auto [statics_ptr, statics] = create_mock<MockStaticsWindowManager>();

    auto windows = register_test_module().with_statics(std::move(statics_ptr)).build();

    std::shared_ptr<IStaticMesh> raised_static;
    auto t1 = windows->on_static_selected += capture(raised_static);

    auto static_mesh = mock_shared<MockStaticMesh>();
    statics.on_static_selected(static_mesh);

    ASSERT_EQ(raised_static, static_mesh);
}

TEST(Windows, StaticsStartup)
{
    auto [statics_ptr, statics] = create_mock<MockStaticsWindowManager>();
    EXPECT_CALL(statics, create_window).Times(1);

    auto windows = register_test_module().with_statics(std::move(statics_ptr)).build();

    windows->setup({ .statics_startup = true });
}

TEST(Windows, TriggersEventsForwarded)
{
    auto [triggers_ptr, triggers] = create_mock<MockTriggersWindowManager>();
    auto waypoint = mock_shared<MockWaypoint>();
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, insert(A<const Vector3&>(), A<const Vector3&>(), A<uint32_t>(), IWaypoint::Type::Trigger, A<uint32_t>())).Times(1).WillRepeatedly(Return(100));
    EXPECT_CALL(*route, waypoint(100)).Times(1).WillRepeatedly(Return(waypoint));

    auto windows = register_test_module().with_triggers(std::move(triggers_ptr)).build();
    windows->set_route(route);

    TokenStore store;
    std::shared_ptr<IItem> raised_item;
    store += windows->on_item_selected += capture(raised_item);
    std::shared_ptr<ITrigger> raised_trigger;
    store += windows->on_trigger_selected += capture(raised_trigger);
    std::shared_ptr<IWaypoint> raised_waypoint;
    store += windows->on_waypoint_selected += capture(raised_waypoint);
    std::shared_ptr<ICameraSink> raised_camera_sink;
    store += windows->on_camera_sink_selected += capture(raised_camera_sink);
    bool raised = false;
    store += windows->on_scene_changed += capture_called(raised);

    auto item = mock_shared<MockItem>();
    auto trigger = mock_shared<MockTrigger>();
    auto camera_sink = mock_shared<MockCameraSink>();

    triggers.on_add_to_route(trigger);
    triggers.on_item_selected(item);
    triggers.on_trigger_selected(trigger);
    triggers.on_scene_changed();
    triggers.on_camera_sink_selected(camera_sink);

    ASSERT_EQ(raised_item, item);
    ASSERT_EQ(raised_trigger, trigger);
    ASSERT_EQ(raised_waypoint, waypoint);
    ASSERT_EQ(raised, true);
    ASSERT_EQ(raised_camera_sink, camera_sink);
}

TEST(Windows, TriggersStartup)
{
    auto [triggers_ptr, triggers] = create_mock<MockTriggersWindowManager>();
    EXPECT_CALL(triggers, create_window).Times(1);

    auto windows = register_test_module().with_triggers(std::move(triggers_ptr)).build();

    windows->setup({ .triggers_startup = true });
}

TEST(Windows, WindowsRendered)
{
    auto [cameras_ptr, cameras] = create_mock<MockCameraSinkWindowManager>();
    EXPECT_CALL(cameras, render).Times(1);
    auto [console_ptr, console] = create_mock<MockConsoleManager>();
    EXPECT_CALL(console, render).Times(1);
    auto [items_ptr, items] = create_mock<MockItemsWindowManager>();
    EXPECT_CALL(items, render).Times(1);
    auto [lights_ptr, lights] = create_mock<MockLightsWindowManager>();
    EXPECT_CALL(lights, render).Times(1);
    auto [log_ptr, log] = create_mock<MockLogWindowManager>();
    EXPECT_CALL(log, render).Times(1);
    auto [plugins_ptr, plugins] = create_mock<MockPluginsWindowManager>();
    EXPECT_CALL(plugins, render).Times(1);
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();
    EXPECT_CALL(rooms, render).Times(1);
    auto [route_ptr, route] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route, render).Times(1);
    auto [sounds_ptr, sounds] = create_mock<MockSoundsWindowManager>();
    EXPECT_CALL(sounds, render).Times(1);
    auto [statics_ptr, statics] = create_mock<MockStaticsWindowManager>();
    EXPECT_CALL(statics, render).Times(1);
    auto [textures_ptr, textures] = create_mock<MockTexturesWindowManager>();
    EXPECT_CALL(textures, render).Times(1);
    auto [triggers_ptr, triggers] = create_mock<MockTriggersWindowManager>();
    EXPECT_CALL(triggers, render).Times(1);
    auto windows = register_test_module()
        .with_camera_sinks(std::move(cameras_ptr))
        .with_console(std::move(console_ptr))
        .with_items(std::move(items_ptr))
        .with_lights(std::move(lights_ptr))
        .with_log(std::move(log_ptr))
        .with_plugins(std::move(plugins_ptr))
        .with_rooms(std::move(rooms_ptr))
        .with_route(std::move(route_ptr))
        .with_sounds(std::move(sounds_ptr))
        .with_statics(std::move(statics_ptr))
        .with_textures(std::move(textures_ptr))
        .with_triggers(std::move(triggers_ptr))
        .build();

    windows->render();
}

TEST(Windows, WindowsUpdated)
{
    auto [cameras_ptr, cameras] = create_mock<MockCameraSinkWindowManager>();
    EXPECT_CALL(cameras, update).Times(1);
    auto [items_ptr, items] = create_mock<MockItemsWindowManager>();
    EXPECT_CALL(items, update).Times(1);
    auto [lights_ptr, lights] = create_mock<MockLightsWindowManager>();
    EXPECT_CALL(lights, update).Times(1);
    auto [plugins_ptr, plugins] = create_mock<MockPluginsWindowManager>();
    EXPECT_CALL(plugins, update).Times(1);
    auto [rooms_ptr, rooms] = create_mock<MockRoomsWindowManager>();
    EXPECT_CALL(rooms, update).Times(1);
    auto [route_ptr, route] = create_mock<MockRouteWindowManager>();
    EXPECT_CALL(route, update).Times(1);
    auto [statics_ptr, statics] = create_mock<MockStaticsWindowManager>();
    EXPECT_CALL(statics, update).Times(1);
    auto [triggers_ptr, triggers] = create_mock<MockTriggersWindowManager>();
    EXPECT_CALL(triggers, update).Times(1);
    auto windows = register_test_module()
        .with_camera_sinks(std::move(cameras_ptr))
        .with_items(std::move(items_ptr))
        .with_lights(std::move(lights_ptr))
        .with_plugins(std::move(plugins_ptr))
        .with_rooms(std::move(rooms_ptr))
        .with_route(std::move(route_ptr))
        .with_statics(std::move(statics_ptr))
        .with_triggers(std::move(triggers_ptr))
        .build();

    windows->update(1.0f);
}

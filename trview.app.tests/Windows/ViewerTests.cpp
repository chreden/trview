#include <trview.app/Windows/Viewer.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.tests.common/Window.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Geometry/IPicking.h>
#include <trview.app/Mocks/UI/IViewerUI.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trview.input/Mocks/IMouse.h>
#include <trview.app/Mocks/Tools/ICompass.h>
#include <trview.app/Mocks/Tools/IMeasure.h>
#include <trview.graphics/mocks/ISprite.h>
#include <trview.graphics/mocks/IRenderTarget.h>
#include <trview.graphics/mocks/IDeviceWindow.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Graphics/ISectorHighlight.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Routing/IWaypoint.h>

using testing::Return;
using namespace trview;
using namespace trview::mocks;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trview::input;
using namespace trview::input::mocks;
using namespace trview::tests;
using namespace DirectX::SimpleMath;

namespace
{
    /// Simulates a context menu activation - 
    void activate_context_menu(MockPicking& picking, MockMouse& mouse, PickResult::Type type, uint32_t index, Vector3 position = Vector3::Zero, Vector3 centroid = Vector3::Zero)
    {
        PickResult pick_result{};
        pick_result.hit = true;
        pick_result.type = type;
        pick_result.index = index;
        pick_result.position = position;
        pick_result.centroid = centroid;
        picking.on_pick({}, pick_result);
        mouse.mouse_click(IMouse::Button::Right);
    }

    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            trview::Window window{ create_test_window(L"ViewerTests") };
            std::shared_ptr<IDevice> device{ std::make_shared<MockDevice>() };
            std::unique_ptr<IViewerUI> ui{ std::make_unique<MockViewerUI>() };
            std::unique_ptr<IPicking> picking{ std::make_unique<MockPicking>() };
            std::unique_ptr<IMouse> mouse{ std::make_unique<MockMouse>() };
            std::shared_ptr<MockShortcuts> shortcuts{ std::make_shared<MockShortcuts>() };
            std::shared_ptr<IRoute> route{ std::make_shared<MockRoute>() };
            ISprite::Source sprite_source{ [](auto&&...) { return std::make_unique<MockSprite>(); }};
            std::unique_ptr<ICompass> compass{ std::make_unique<MockCompass>() };
            std::unique_ptr<IMeasure> measure{ std::make_unique<MockMeasure>() };
            IRenderTarget::SizeSource render_target_source{ [](auto&&...) { return std::make_unique<MockRenderTarget>(); } };
            IDeviceWindow::Source device_window_source{ [](auto&&...) { return std::make_unique<MockDeviceWindow>(); } };
            std::unique_ptr<ISectorHighlight> sector_highlight{ std::make_unique<MockSectorHighlight>() };

            std::unique_ptr<Viewer> build()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
                return std::make_unique<Viewer>(window, device, std::move(ui), std::move(picking), std::move(mouse), shortcuts, route, sprite_source,
                    std::move(compass), std::move(measure), render_target_source, device_window_source, std::move(sector_highlight));
            }

            test_module& with_ui(std::unique_ptr<IViewerUI> ui)
            {
                this->ui = std::move(ui);
                return *this;
            }

            test_module& with_picking(std::unique_ptr<IPicking> picking)
            {
                this->picking = std::move(picking);
                return *this;
            }

            test_module& with_mouse(std::unique_ptr<IMouse> mouse)
            {
                this->mouse = std::move(mouse);
                return *this;
            }
        };
        return test_module{};
    }
}

/// Tests that the on_select_item event from the UI is observed and forwarded.
TEST(Viewer, SelectItemRaisedForValidItem)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();

    Item item(123, 0, 0, L"Test", 0, 0, {}, Vector3::Zero);
    MockLevel level;

    std::vector<Item> items_list{ item };
    EXPECT_CALL(level, items).WillRepeatedly([&]() { return items_list; });

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();
    viewer->open(&level);

    std::optional<Item> raised_item;
    auto token = viewer->on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    ui.on_select_item(0);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 123);
}

/// Tests that the on_hide event from the UI is observed but not forwarded when the item is invalid.
TEST(Viewer, SelectItemNotRaisedForInvalidItem)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    std::optional<Item> raised_item;
    auto token = viewer->on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    ui.on_select_item(0);

    ASSERT_FALSE(raised_item.has_value());
}

/// Tests that the on_hide event from the UI is observed and forwarded when the item is valid.
TEST(Viewer, ItemVisibilityRaisedForValidItem)
{
    Item item(123, 0, 0, L"Test", 0, 0, {}, Vector3::Zero);
    MockLevel level;

    std::vector<Item> items_list{ item };
    EXPECT_CALL(level, items).WillRepeatedly([&]() { return items_list; });

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();

    viewer->open(&level);

    std::optional<std::tuple<Item, bool>> raised_item;
    auto token = viewer->on_item_visibility += [&raised_item](const auto& item, auto visible) { raised_item = { item, visible }; };

    activate_context_menu(picking, mouse, PickResult::Type::Entity, 0);

    ui.on_hide();

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(std::get<0>(raised_item.value()).number(), 123);
    ASSERT_FALSE(std::get<1>(raised_item.value()));
}

/// Tests that the on_settings event from the UI is observed and forwarded.
TEST(Viewer, SettingsRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    std::optional<UserSettings> raised_settings;
    auto token = viewer->on_settings += [&raised_settings](const auto& settings) { raised_settings = settings; };

    UserSettings settings;
    settings.add_recent_file("test file");
    ui.on_settings(settings);

    ASSERT_TRUE(raised_settings.has_value());
    ASSERT_EQ(raised_settings.value().recent_files.size(), 1);
    ASSERT_EQ(raised_settings.value().recent_files.front(), "test file");
}

/// Tests that the on_select_room event from the UI is observed and forwarded.
TEST(Viewer, SelectRoomRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    std::optional<uint32_t> raised_room;
    auto token = viewer->on_room_selected += [&raised_room](const auto& room) { raised_room = room; };

    ui.on_select_room(0);

    ASSERT_TRUE(raised_room.has_value());
    ASSERT_EQ(raised_room.value(), 0u);
}

/// Tests that the trigger selected event is raised when the user clicks on a trigger.
TEST(Viewer, SelectTriggerRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();

    MockLevel level;
    auto trigger = std::make_shared<MockTrigger>();
    std::vector<std::weak_ptr<ITrigger>> triggers_list(101);
    triggers_list[100] = trigger;

    EXPECT_CALL(level, triggers).WillRepeatedly([&]() { return triggers_list; });

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(&level);

    std::optional<std::weak_ptr<ITrigger>> selected_trigger;
    auto token = viewer->on_trigger_selected += [&selected_trigger](const auto& trigger) { selected_trigger = trigger; };

    activate_context_menu(picking, mouse, PickResult::Type::Trigger, 100);
    mouse.mouse_click(IMouse::Button::Left);

    ASSERT_TRUE(selected_trigger.has_value());
    ASSERT_EQ(selected_trigger.value().lock(), trigger);
}

/// Tests that the on_hide event from the UI is observed and forwarded for triggers.
TEST(Viewer, TriggerVisibilityRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();

    MockLevel level;
    std::vector<std::weak_ptr<ITrigger>> triggers_list(101);
    auto trigger = std::make_shared<MockTrigger>();
    triggers_list[100] = trigger;

    EXPECT_CALL(level, triggers).WillRepeatedly([&]() { return triggers_list; });

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(&level);

    std::optional<std::tuple<std::weak_ptr<ITrigger>, bool>> raised_trigger;
    auto token = viewer->on_trigger_visibility += [&raised_trigger](const auto& trigger, auto visible) { raised_trigger = { trigger, visible }; };

    activate_context_menu(picking, mouse, PickResult::Type::Trigger, 100);

    ui.on_hide();

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(std::get<0>(raised_trigger.value()).lock(), trigger);
    ASSERT_FALSE(std::get<1>(raised_trigger.value()));
}

/// Tests that the waypoint selected event is raised when the user clicks on a waypoint.
TEST(Viewer, SelectWaypointRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();

    std::optional<uint32_t> selected_waypoint;
    auto token = viewer->on_waypoint_selected += [&selected_waypoint](const auto& waypoint) { selected_waypoint = waypoint; };

    activate_context_menu(picking, mouse, PickResult::Type::Waypoint, 100);
    mouse.mouse_click(IMouse::Button::Left);

    ASSERT_TRUE(selected_waypoint.has_value());
    ASSERT_EQ(selected_waypoint.value(), 100u);
}

/// Tests that the on_remove_waypoint event from the UI is observed and forwarded.
TEST(Viewer, RemoveWaypointRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();

    std::optional<uint32_t> removed_waypoint;
    auto token = viewer->on_waypoint_removed += [&removed_waypoint](const auto& waypoint) { removed_waypoint = waypoint; };

    activate_context_menu(picking, mouse, PickResult::Type::Waypoint, 100);

    ui.on_remove_waypoint();

    ASSERT_TRUE(removed_waypoint.has_value());
    ASSERT_EQ(removed_waypoint.value(), 100u);
}

/// Tests that the on_add_waypoint event from the UI is observed and forwarded.
TEST(Viewer, AddWaypointRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();

    MockLevel level;
    std::vector<Item> items_list(51);
    Item item(50, 10, 0, L"Test", 0, 0, {}, Vector3::Zero);
    items_list[50] = item;
    EXPECT_CALL(level, items).WillRepeatedly([&]() { return items_list; });

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(&level);

    std::optional<std::tuple<Vector3, Vector3, uint32_t, IWaypoint::Type, uint32_t>> added_waypoint;
    auto token = viewer->on_waypoint_added += [&added_waypoint](const auto& position, const auto& normal, uint32_t room, IWaypoint::Type type, uint32_t index)
    {
        added_waypoint = { position, normal, room, type, index };
    };

    activate_context_menu(picking, mouse, PickResult::Type::Entity, 50);

    ui.on_add_waypoint();

    ASSERT_TRUE(added_waypoint.has_value());
    ASSERT_EQ(std::get<2>(added_waypoint.value()), 10u);
    ASSERT_EQ(std::get<3>(added_waypoint.value()), IWaypoint::Type::Entity);
    ASSERT_EQ(std::get<4>(added_waypoint.value()), 50u);
}

/// Tests that right clicking activates the context menu.
TEST(Viewer, RightClickActivatesContextMenu)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();

    EXPECT_CALL(ui, set_show_context_menu(false));
    EXPECT_CALL(ui, set_show_context_menu(true)).Times(1);

    activate_context_menu(picking, mouse, PickResult::Type::Room, 0);
}

TEST(Viewer, OrbitEnabledWhenItemSelectedAndAutoOrbitEnabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(CameraMode::Orbit)).Times(2);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    UserSettings settings;
    settings.auto_orbit = true;
    viewer->set_settings(settings);

    viewer->set_camera_mode(CameraMode::Free);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);

    viewer->select_item({});
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Orbit);
}

TEST(Viewer, OrbitNotEnabledWhenItemSelectedAndAutoOrbitDisabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(CameraMode::Orbit)).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    UserSettings settings;
    settings.auto_orbit = false;
    viewer->set_settings(settings);

    viewer->set_camera_mode(CameraMode::Free);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);

    viewer->select_item({});
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);
}

TEST(Viewer, OrbitEnabledWhenTriggerSelectedAndAutoOrbitEnabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(CameraMode::Orbit)).Times(2);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    UserSettings settings;
    settings.auto_orbit = true;
    viewer->set_settings(settings);

    viewer->set_camera_mode(CameraMode::Free);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);

    auto trigger = std::make_shared<MockTrigger>();
    viewer->select_trigger(trigger);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Orbit);
}

TEST(Viewer, OrbitNotEnabledWhenTriggerSelectedAndAutoOrbitDisabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(CameraMode::Orbit)).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    UserSettings settings;
    settings.auto_orbit = false;
    viewer->set_settings(settings);

    viewer->set_camera_mode(CameraMode::Free);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);

    auto trigger = std::make_shared<MockTrigger>();
    viewer->select_trigger(trigger);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);
}

TEST(Viewer, OrbitEnabledWhenWaypointSelectedAndAutoOrbitEnabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(CameraMode::Orbit)).Times(2);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    UserSettings settings;
    settings.auto_orbit = true;
    viewer->set_settings(settings);

    viewer->set_camera_mode(CameraMode::Free);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);

    auto mesh = std::make_shared<MockMesh>();
    viewer->select_waypoint(MockWaypoint{});
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Orbit);
}

TEST(Viewer, OrbitNotEnabledWhenWaypointSelectedAndAutoOrbitDisabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(CameraMode::Orbit)).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    UserSettings settings;
    settings.auto_orbit = false;
    viewer->set_settings(settings);

    viewer->set_camera_mode(CameraMode::Free);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);

    auto mesh = std::make_shared<MockMesh>();
    viewer->select_waypoint(MockWaypoint{});
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);
}


TEST(Viewer, OrbitEnabledWhenRoomSelectedAndAutoOrbitEnabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(CameraMode::Orbit)).Times(2);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    UserSettings settings;
    settings.auto_orbit = true;
    viewer->set_settings(settings);

    auto [level_ptr, level] = create_mock<MockLevel>();
    auto room = std::make_shared<MockRoom>();

    EXPECT_CALL(level, number_of_rooms).WillRepeatedly(Return(1));
    EXPECT_CALL(level, rooms).WillRepeatedly(Return(std::vector<std::weak_ptr<IRoom>>{ room }));
    EXPECT_CALL(level, room).WillRepeatedly(Return(room));
    viewer->open(&level);

    viewer->set_camera_mode(CameraMode::Free);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);

    viewer->select_room(0);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Orbit);
}

TEST(Viewer, OrbitNotEnabledWhenRoomSelectedAndAutoOrbitDisabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(CameraMode::Orbit)).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    UserSettings settings;
    settings.auto_orbit = false;
    viewer->set_settings(settings);

    auto [level_ptr, level] = create_mock<MockLevel>();
    auto room = std::make_shared<MockRoom>();

    EXPECT_CALL(level, number_of_rooms).WillRepeatedly(Return(1));
    EXPECT_CALL(level, rooms).WillRepeatedly(Return(std::vector<std::weak_ptr<IRoom>>{ room }));
    EXPECT_CALL(level, room).WillRepeatedly(Return(room));
    viewer->open(&level);

    viewer->set_camera_mode(CameraMode::Free);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);

    viewer->select_room(0);
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);
}

TEST(Viewer, SetSettingsUpdatesUI)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_settings).Times(2);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();
    viewer->set_settings({});
}

TEST(Viewer, CameraRotationUpdated)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    auto& camera = viewer->current_camera();
    camera.set_rotation_yaw(0);
    camera.set_rotation_pitch(0.5f);
    ASSERT_FLOAT_EQ(0, camera.rotation_yaw());
    ASSERT_FLOAT_EQ(0.5f, camera.rotation_pitch());

    ui.on_camera_rotation(2.0f, 1.0f);
    ASSERT_FLOAT_EQ(2.0f, camera.rotation_yaw());
    ASSERT_FLOAT_EQ(1.0f, camera.rotation_pitch());
}

TEST(Viewer, SetShowBoundingBox)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [level_ptr, level] = create_mock<MockLevel>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    EXPECT_CALL(level, set_show_bounding_boxes(false)).Times(1);
    EXPECT_CALL(ui, set_show_bounding_boxes(true)).Times(1);
    EXPECT_CALL(level, set_show_bounding_boxes(true)).Times(1);

    viewer->open(&level);
    ui.on_show_bounding_boxes(true);
}

TEST(Viewer, OnAddWaypointRaisedForWaypoint)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    bool raised = false;
    auto token = viewer->on_waypoint_added += [&](auto&&...)
    {
        raised = true;
    };

    ui.on_add_waypoint();
    ASSERT_TRUE(raised);
}

TEST(Viewer, OnAddWaypointRaisedForMidWaypoint)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    bool raised = false;
    auto token = viewer->on_waypoint_added += [&](auto&&...)
    {
        raised = true;
    };

    ui.on_add_mid_waypoint();
    ASSERT_TRUE(raised);
}

TEST(Viewer, WaypointUsesPosition)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();

    std::optional<Vector3> raised_position;
    auto token = viewer->on_waypoint_added += [&](auto&& position, auto&&...)
    {
        raised_position = position;
    };

    activate_context_menu(picking, mouse, PickResult::Type::Room, 0, Vector3(1, 2, 3), Vector3(3, 4, 5));

    ui.on_add_waypoint();
    ASSERT_TRUE(raised_position.has_value());
    ASSERT_EQ(raised_position.value(), Vector3(1, 2, 3));
}

TEST(Viewer, MidWaypointUsesCentroid)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();

    std::optional<Vector3> raised_position;
    auto token = viewer->on_waypoint_added += [&](auto&& position, auto&&...)
    {
        raised_position = position;
    };

    activate_context_menu(picking, mouse, PickResult::Type::Room, 0, Vector3(1, 2, 3), Vector3(3, 4, 5));

    ui.on_add_mid_waypoint();
    ASSERT_TRUE(raised_position.has_value());
    ASSERT_EQ(raised_position.value(), Vector3(3, 4, 5));
}

TEST(Viewer, OnRandoLocationCreatesWaypoint)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();

    std::optional<IWaypoint::Type> raised_type;
    auto token = viewer->on_waypoint_added += [&](auto&& position, auto&& normal, auto&& index, auto&& type, auto&&...)
    {
        raised_type = type;
    };

    activate_context_menu(picking, mouse, PickResult::Type::Room, 0, Vector3(1, 2, 3), Vector3(3, 4, 5));

    ui.on_rando_location();
    ASSERT_TRUE(raised_type.has_value());
    ASSERT_EQ(raised_type.value(), IWaypoint::Type::RandoLocation);
}

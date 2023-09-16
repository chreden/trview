#include <trview.app/Windows/Viewer.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.tests.common/Window.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/ICameraSink.h>
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
#include <trview.app/Mocks/Elements/ILight.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include "TestImgui.h"

using testing::A;
using testing::Return;
using testing::NiceMock;
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
            std::shared_ptr<IDevice> device{ mock_shared<MockDevice>() };
            std::unique_ptr<IViewerUI> ui{ mock_unique<MockViewerUI>() };
            std::unique_ptr<IPicking> picking{ mock_unique<MockPicking>() };
            std::unique_ptr<IMouse> mouse{ mock_unique<MockMouse>() };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            std::shared_ptr<IRoute> route{ mock_shared<MockRoute>() };
            ISprite::Source sprite_source{ [](auto&&...) { return mock_unique<MockSprite>(); }};
            std::unique_ptr<ICompass> compass{ mock_unique<MockCompass>() };
            std::unique_ptr<IMeasure> measure{ mock_unique<MockMeasure>() };
            IRenderTarget::SizeSource render_target_source{ [](auto&&...) { return mock_unique<MockRenderTarget>(); } };
            IDeviceWindow::Source device_window_source{ [](auto&&...) { return mock_unique<MockDeviceWindow>(); } };
            std::unique_ptr<ISectorHighlight> sector_highlight{ mock_unique<MockSectorHighlight>() };
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };

            std::unique_ptr<Viewer> build()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
                return std::make_unique<Viewer>(window, device, std::move(ui), std::move(picking), std::move(mouse), shortcuts, route, sprite_source,
                    std::move(compass), std::move(measure), render_target_source, device_window_source, std::move(sector_highlight), clipboard);
            }

            test_module& with_device(const std::shared_ptr<IDevice>& device)
            {
                this->device = device;
                return *this;
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

            test_module& with_clipboard(std::shared_ptr<IClipboard> clipboard)
            {
                this->clipboard = clipboard;
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

    auto item = mock_shared<MockItem>();
    NiceMock<MockLevel> level;
    EXPECT_CALL(level, item(123)).WillRepeatedly(Return(item));

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();
    viewer->open(&level, ILevel::OpenMode::Full);

    std::shared_ptr<IItem> raised_item;
    auto token = viewer->on_item_selected += [&raised_item](const auto& item) { raised_item = item.lock(); };

    ui.on_select_item(123);

    ASSERT_TRUE(raised_item);
    ASSERT_EQ(raised_item, item);
}

/// Tests that the on_hide event from the UI is observed but not forwarded when the item is invalid.
TEST(Viewer, SelectItemNotRaisedForInvalidItem)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    std::shared_ptr<IItem> raised_item;
    auto token = viewer->on_item_selected += [&raised_item](const auto& item) { raised_item = item.lock(); };

    ui.on_select_item(0);

    ASSERT_FALSE(raised_item);
}

/// Tests that the on_hide event from the UI is observed and forwarded when the item is valid.
TEST(Viewer, ItemVisibilityRaisedForValidItem)
{
    auto item = mock_shared<MockItem>();
    NiceMock<MockLevel> level;
    EXPECT_CALL(level, item(123)).WillRepeatedly(Return(item));

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    TestImgui imgui;

    viewer->open(&level, ILevel::OpenMode::Full);

    std::optional<std::tuple<std::shared_ptr<IItem>, bool>> raised_item;
    auto token = viewer->on_item_visibility += [&raised_item](const auto& item, auto visible) { raised_item = { item.lock(), visible }; };

    activate_context_menu(picking, mouse, PickResult::Type::Entity, 123);

    ui.on_hide();

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(std::get<0>(raised_item.value()), item);
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

    NiceMock<MockLevel> level;
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(level, trigger(100)).WillRepeatedly(Return(trigger));

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(&level, ILevel::OpenMode::Full);
    TestImgui imgui;

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
    TestImgui imgui;

    NiceMock<MockLevel> level;
    auto trigger = mock_shared<MockTrigger>();
    EXPECT_CALL(level, trigger(100)).WillRepeatedly(Return(trigger));

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(&level, ILevel::OpenMode::Full);

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
    TestImgui imgui;

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
    TestImgui imgui;

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
    TestImgui imgui;

    NiceMock<MockLevel> level;
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(&level, ILevel::OpenMode::Full);

    std::optional<std::tuple<Vector3, Vector3, uint32_t, IWaypoint::Type, uint32_t>> added_waypoint;
    auto token = viewer->on_waypoint_added += [&added_waypoint](const auto& position, const auto& normal, uint32_t room, IWaypoint::Type type, uint32_t index)
    {
        added_waypoint = { position, normal, room, type, index };
    };

    activate_context_menu(picking, mouse, PickResult::Type::Room, 50, Vector3(100, 200, 300));

    ui.on_add_waypoint();

    ASSERT_TRUE(added_waypoint.has_value());
    ASSERT_EQ(std::get<0>(added_waypoint.value()), Vector3(100, 200, 300));
    ASSERT_EQ(std::get<2>(added_waypoint.value()), 50u);
    ASSERT_EQ(std::get<3>(added_waypoint.value()), IWaypoint::Type::Position);
    ASSERT_EQ(std::get<4>(added_waypoint.value()), 50u);
}

TEST(Viewer, AddWaypointRaisedUsesItemPosition)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    TestImgui imgui;

    NiceMock<MockLevel> level;
    auto item = mock_shared<MockItem>()->with_room(mock_shared<MockRoom>()->with_number(10))->with_number(50);

    EXPECT_CALL(level, item(50)).WillRepeatedly(Return(item));

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(&level, ILevel::OpenMode::Full);

    std::optional<std::tuple<Vector3, Vector3, uint32_t, IWaypoint::Type, uint32_t>> added_waypoint;
    auto token = viewer->on_waypoint_added += [&added_waypoint](const auto& position, const auto& normal, uint32_t room, IWaypoint::Type type, uint32_t index)
    {
        added_waypoint = { position, normal, room, type, index };
    };

    activate_context_menu(picking, mouse, PickResult::Type::Entity, 50, Vector3(100, 200, 300));

    ui.on_add_waypoint();

    ASSERT_TRUE(added_waypoint.has_value());
    ASSERT_EQ(std::get<0>(added_waypoint.value()), Vector3::Zero);
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
    TestImgui imgui;

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

    viewer->select_item(mock_shared<MockItem>());
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

    auto trigger = mock_shared<MockTrigger>();
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

    auto trigger = mock_shared<MockTrigger>();
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

    auto mesh = mock_shared<MockMesh>();
    viewer->select_waypoint(mock_shared<MockWaypoint>());
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

    auto mesh = mock_shared<MockMesh>();
    viewer->select_waypoint(mock_shared<MockWaypoint>());
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
    auto room = mock_shared<MockRoom>();

    EXPECT_CALL(level, number_of_rooms).WillRepeatedly(Return(1));
    EXPECT_CALL(level, rooms).WillRepeatedly(Return(std::vector<std::weak_ptr<IRoom>>{ room }));
    EXPECT_CALL(level, room).WillRepeatedly(Return(room));
    viewer->open(&level, ILevel::OpenMode::Full);

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
    auto room = mock_shared<MockRoom>();

    EXPECT_CALL(level, number_of_rooms).WillRepeatedly(Return(1));
    EXPECT_CALL(level, rooms).WillRepeatedly(Return(std::vector<std::weak_ptr<IRoom>>{ room }));
    EXPECT_CALL(level, room).WillRepeatedly(Return(room));
    viewer->open(&level, ILevel::OpenMode::Full);

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
    EXPECT_CALL(ui, set_toggle(testing::A<const std::string&>(), testing::A<bool>())).Times(testing::AtLeast(0));
    EXPECT_CALL(ui, set_toggle(IViewer::Options::show_bounding_boxes, true)).Times(1);
    EXPECT_CALL(level, set_show_bounding_boxes(true)).Times(1);

    viewer->open(&level, ILevel::OpenMode::Full);
    ui.on_toggle_changed(IViewer::Options::show_bounding_boxes, true);
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
    TestImgui imgui;

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
    TestImgui imgui;

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

TEST(Viewer, DepthViewOptionUpdatesLevel)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    NiceMock<MockLevel> level;
    EXPECT_CALL(level, set_neighbour_depth(6)).Times(1);
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();
    viewer->open(&level, ILevel::OpenMode::Full);
    ui.on_scalar_changed(IViewer::Options::depth, 6);
}

TEST(Viewer, SetShowItems)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [level_ptr, level] = create_mock<MockLevel>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    EXPECT_CALL(level, set_show_items(false)).Times(1);
    EXPECT_CALL(ui, set_toggle(testing::A<const std::string&>(), testing::A<bool>())).Times(testing::AtLeast(0));
    EXPECT_CALL(ui, set_toggle(IViewer::Options::items, true)).Times(1);
    EXPECT_CALL(level, set_show_items(true)).Times(1);

    viewer->open(&level, ILevel::OpenMode::Full);
    ui.on_toggle_changed(IViewer::Options::items, true);
}

TEST(Viewer, SelectionRendered)
{
    auto device = mock_shared<MockDevice>();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context{ new NiceMock<MockD3D11DeviceContext>() };
    EXPECT_CALL(*device, context).WillRepeatedly(Return(context));

    auto viewer = register_test_module().with_device(device).build();

    auto texture_storage = mock_shared<MockLevelTextureStorage>();
    NiceMock<MockLevel> level;
    ON_CALL(level, texture_storage).WillByDefault(testing::Return(texture_storage));
    EXPECT_CALL(level, render(A<const ICamera&>(), true)).Times(1);
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, render(A<const ICamera&>(), A<const ILevelTextureStorage&>(), true)).Times(1);

    viewer->open(&level, ILevel::OpenMode::Full);
    viewer->set_route(route);
    viewer->render();
}

TEST(Viewer, SelectionNotRendered)
{
    auto device = mock_shared<MockDevice>();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context{ new NiceMock<MockD3D11DeviceContext>() };
    EXPECT_CALL(*device, context).WillRepeatedly(Return(context));

    auto viewer = register_test_module().with_device(device).build();

    auto texture_storage = mock_shared<MockLevelTextureStorage>();
    NiceMock<MockLevel> level;
    ON_CALL(level, texture_storage).WillByDefault(testing::Return(texture_storage));
    EXPECT_CALL(level, render(A<const ICamera&>(), false)).Times(1);
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, render(A<const ICamera&>(), A<const ILevelTextureStorage&>(), false)).Times(1);

    viewer->set_show_selection(false);
    viewer->open(&level, ILevel::OpenMode::Full);
    viewer->set_route(route);
    viewer->render();
}

TEST(Viewer, LightVisibilityRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    TestImgui imgui;

    NiceMock<MockLevel> level;
    auto light = mock_shared<MockLight>();
    EXPECT_CALL(level, light(100)).WillRepeatedly(Return(light));

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(&level, ILevel::OpenMode::Full);

    std::optional<std::tuple<std::weak_ptr<ILight>, bool>> raised_light;
    auto token = viewer->on_light_visibility += [&raised_light](const auto& light, auto visible) { raised_light = { light, visible }; };

    activate_context_menu(picking, mouse, PickResult::Type::Light, 100);

    ui.on_hide();

    ASSERT_TRUE(raised_light.has_value());
    ASSERT_EQ(std::get<0>(raised_light.value()).lock(), light);
    ASSERT_FALSE(std::get<1>(raised_light.value()));
}

TEST(Viewer, RoomVisibilityRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    TestImgui imgui;

    NiceMock<MockLevel> level;
    auto room = mock_shared<MockRoom>();
    EXPECT_CALL(level, room(100)).WillRepeatedly(Return(room));

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(&level, ILevel::OpenMode::Full);

    std::optional<std::tuple<std::weak_ptr<IRoom>, bool>> raised_room;
    auto token = viewer->on_room_visibility += [&raised_room](const auto& room, auto visible) { raised_room = { room, visible }; };

    activate_context_menu(picking, mouse, PickResult::Type::Room, 100);

    ui.on_hide();

    ASSERT_TRUE(raised_room.has_value());
    ASSERT_EQ(std::get<0>(raised_room.value()).lock(), room);
    ASSERT_FALSE(std::get<1>(raised_room.value()));
}

TEST(Viewer, OrbitHereOrbitsWhenSettingDisabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode(CameraMode::Free)).Times(1);
    EXPECT_CALL(ui, set_camera_mode(CameraMode::Orbit)).Times(2);

    UserSettings settings;
    settings.auto_orbit = false;

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();
    viewer->set_camera_mode(CameraMode::Free);
    viewer->set_settings(settings);

    ASSERT_EQ(viewer->camera_mode(), CameraMode::Free);

    ui.on_orbit();
    ASSERT_EQ(viewer->camera_mode(), CameraMode::Orbit);
}

TEST(Viewer, ReloadLevelSyncProperties)
{
    std::set<uint32_t> groups{ 1, 2, 3 };

    auto [original_ptr, original] = create_mock<MockLevel>();
    EXPECT_CALL(original, alternate_mode).WillRepeatedly(Return(true));
    EXPECT_CALL(original, neighbour_depth).WillRepeatedly(Return(6));
    EXPECT_CALL(original, highlight_mode_enabled(ILevel::RoomHighlightMode::Highlight)).WillRepeatedly(Return(true));
    EXPECT_CALL(original, highlight_mode_enabled(ILevel::RoomHighlightMode::Neighbours)).WillRepeatedly(Return(true));
    EXPECT_CALL(original, alternate_group(1)).WillRepeatedly(Return(true));
    EXPECT_CALL(original, alternate_group(2)).WillRepeatedly(Return(true));
    EXPECT_CALL(original, alternate_group(3)).WillRepeatedly(Return(true));

    auto [reloaded_ptr, reloaded] = create_mock<MockLevel>();
    EXPECT_CALL(reloaded, set_alternate_mode(true)).Times(1);
    EXPECT_CALL(reloaded, set_neighbour_depth(6)).Times(1);
    EXPECT_CALL(reloaded, set_highlight_mode(ILevel::RoomHighlightMode::Neighbours, true)).Times(1);
    EXPECT_CALL(reloaded, set_highlight_mode(ILevel::RoomHighlightMode::Highlight, true)).Times(1);
    EXPECT_CALL(reloaded, set_alternate_group(1, true)).Times(1);
    EXPECT_CALL(reloaded, set_alternate_group(2, true)).Times(1);
    EXPECT_CALL(reloaded, set_alternate_group(3, true)).Times(1);
    EXPECT_CALL(reloaded, alternate_groups).WillRepeatedly(Return(groups));

    auto viewer = register_test_module().build();

    viewer->open(&original, ILevel::OpenMode::Full);
    viewer->open(&reloaded, ILevel::OpenMode::Reload);
}

TEST(Viewer, CopyPosition)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto clipboard = mock_shared<MockClipboard>();
    TestImgui imgui;

    EXPECT_CALL(*clipboard, write(std::wstring(L"1024, 2048, 3072"))).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_clipboard(clipboard).with_mouse(std::move(mouse_ptr)).build();

    activate_context_menu(picking, mouse, PickResult::Type::Room, 0, { 1, 2, 3 });

    ui.on_copy(trview::IContextMenu::CopyType::Position);
}

TEST(Viewer, CopyRoom)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto clipboard = mock_shared<MockClipboard>();
    TestImgui imgui;

    EXPECT_CALL(*clipboard, write(std::wstring(L"14"))).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_clipboard(clipboard).with_mouse(std::move(mouse_ptr)).build();

    activate_context_menu(picking, mouse, PickResult::Type::Room, 14);

    ui.on_copy(trview::IContextMenu::CopyType::Number);
}

TEST(Viewer, SetTriggeredBy)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto clipboard = mock_shared<MockClipboard>();
    TestImgui imgui;

    EXPECT_CALL(ui, set_triggered_by).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_clipboard(clipboard).with_mouse(std::move(mouse_ptr)).build();

    auto level = mock_shared<MockLevel>();
    viewer->open(level.get(), ILevel::OpenMode::Full);

    activate_context_menu(picking, mouse, PickResult::Type::Entity, 14);
}


TEST(Viewer, SetShowRooms)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [level_ptr, level] = create_mock<MockLevel>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    EXPECT_CALL(level, set_show_rooms(false)).Times(1);
    EXPECT_CALL(ui, set_toggle(testing::A<const std::string&>(), testing::A<bool>())).Times(testing::AtLeast(0));
    EXPECT_CALL(ui, set_toggle(IViewer::Options::rooms, true)).Times(1);
    EXPECT_CALL(level, set_show_rooms(true)).Times(1);

    viewer->open(&level, ILevel::OpenMode::Full);
    ui.on_toggle_changed(IViewer::Options::rooms, true);
}

TEST(Viewer, GoToLaraSelectsLast)
{
    auto [level_ptr, level] = create_mock<MockLevel>();
    auto viewer = register_test_module().build();

    auto item1 = mock_shared<MockItem>();
    auto item2 = mock_shared<MockItem>();
    ON_CALL(level, items).WillByDefault(Return(std::vector<std::weak_ptr<IItem>>{ item1, item2 }));

    std::shared_ptr<IItem> selected;
    auto token = viewer->on_item_selected += [&](const auto& item)
    {
        selected = item.lock();
    };

    viewer->open(&level, ILevel::OpenMode::Full);

    ASSERT_TRUE(selected);
    ASSERT_EQ(selected, item2);
}

TEST(Viewer, CameraSinkVisibilityRaisedForValidItem)
{
    auto cs = mock_shared<MockCameraSink>();
    NiceMock<MockLevel> level;
    EXPECT_CALL(level, camera_sink(123)).WillRepeatedly(Return(cs));
    TestImgui imgui;

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();

    viewer->open(&level, ILevel::OpenMode::Full);

    std::optional<std::tuple<std::shared_ptr<ICameraSink>, bool>> raised;
    auto token = viewer->on_camera_sink_visibility += [&raised](const auto& camera_sink, auto visible)
    { 
        raised = { camera_sink.lock(), visible };
    };

    activate_context_menu(picking, mouse, PickResult::Type::CameraSink, 123);

    ui.on_hide();

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(std::get<0>(raised.value()), cs);
    ASSERT_FALSE(std::get<1>(raised.value()));
}

TEST(Viewer, SetShowCameraSinks)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [level_ptr, level] = create_mock<MockLevel>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    EXPECT_CALL(level, set_show_camera_sinks(false)).Times(1);
    EXPECT_CALL(ui, set_toggle(testing::A<const std::string&>(), testing::A<bool>())).Times(testing::AtLeast(0));
    EXPECT_CALL(ui, set_toggle(IViewer::Options::camera_sinks, true)).Times(1);
    EXPECT_CALL(level, set_show_camera_sinks(true)).Times(1);

    viewer->open(&level, ILevel::OpenMode::Full);
    ui.on_toggle_changed(IViewer::Options::camera_sinks, true);
}

TEST(Viewer, SetTriggeredByCameraSink)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto clipboard = mock_shared<MockClipboard>();
    TestImgui imgui;

    EXPECT_CALL(ui, set_triggered_by).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_clipboard(clipboard).with_mouse(std::move(mouse_ptr)).build();

    auto level = mock_shared<MockLevel>();
    viewer->open(level.get(), ILevel::OpenMode::Full);

    activate_context_menu(picking, mouse, PickResult::Type::CameraSink, 14);
}

TEST(Viewer, SetShowLighting)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [level_ptr, level] = create_mock<MockLevel>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    EXPECT_CALL(level, set_show_lighting(false)).Times(1);
    EXPECT_CALL(ui, set_toggle(testing::A<const std::string&>(), testing::A<bool>())).Times(testing::AtLeast(0));
    EXPECT_CALL(ui, set_toggle(IViewer::Options::lighting, true)).Times(1);
    EXPECT_CALL(level, set_show_lighting(true)).Times(1);

    viewer->open(&level, ILevel::OpenMode::Full);
    ui.on_toggle_changed(IViewer::Options::lighting, true);
}

TEST(Viewer, RoomSelectedForwarded)
{
    auto [level_ptr, level] = create_mock<MockLevel>();
    auto viewer = register_test_module().build();

    std::optional<uint32_t> raised;
    auto token = viewer->on_room_selected += [&](auto r) { raised = r; };

    viewer->open(&level, ILevel::OpenMode::Full);
    level.on_room_selected(123);

    ASSERT_TRUE(raised);
    ASSERT_EQ(*raised, 123);

    auto [new_level_ptr, new_level] = create_mock<MockLevel>();
    viewer->open(&new_level, ILevel::OpenMode::Full);

    raised.reset();
    level.on_room_selected(456);
    ASSERT_FALSE(raised);
}

TEST(Viewer, ItemSelectedForwarded)
{
    auto item = mock_shared<MockItem>();

    auto [level_ptr, level] = create_mock<MockLevel>();
    auto viewer = register_test_module().build();

    std::shared_ptr<IItem> raised;
    auto token = viewer->on_item_selected += [&](auto i) { raised = i.lock(); };

    viewer->open(&level, ILevel::OpenMode::Full);
    level.on_item_selected(item);

    ASSERT_EQ(raised, item);

    raised.reset();
    auto [new_level_ptr, new_level] = create_mock<MockLevel>();
    viewer->open(&new_level, ILevel::OpenMode::Full);
    level.on_item_selected(item);
    ASSERT_EQ(raised, nullptr);
}

TEST(Viewer, TriggerSelectedForwarded)
{
    auto trigger = mock_shared<MockTrigger>();

    auto [level_ptr, level] = create_mock<MockLevel>();
    auto viewer = register_test_module().build();

    std::shared_ptr<ITrigger> raised;
    auto token = viewer->on_trigger_selected += [&](auto t) { raised = t.lock(); };

    viewer->open(&level, ILevel::OpenMode::Full);
    level.on_trigger_selected(trigger);

    ASSERT_EQ(raised, trigger);

    raised.reset();
    auto [new_level_ptr, new_level] = create_mock<MockLevel>();
    viewer->open(&new_level, ILevel::OpenMode::Full);
    level.on_trigger_selected(trigger);
    ASSERT_EQ(raised, nullptr);
}

TEST(Viewer, SetRouteForwarded)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_route).Times(2);
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();
    viewer->set_route(mock_shared<MockRoute>());
}

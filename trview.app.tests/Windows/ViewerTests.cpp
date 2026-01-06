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
#include <trview.graphics/mocks/IDeviceWindow.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Graphics/ISectorHighlight.h>
#include <trlevel/Mocks/ILevel.h>
#include <trview.app/Mocks/Routing/IWaypoint.h>
#include <trview.app/Mocks/Elements/ILight.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.app/Mocks/Elements/ISector.h>
#include <trview.app/Mocks/Elements/ISoundSource.h>
#include <trview.graphics/Mocks/ISamplerState.h>
#include <trview.tests.common/Event.h>
#include <trview.tests.common/Messages.h>
#include <trview.common/Messages/Message.h>
#include <trview.common/Mocks/Messages/IMessageSystem.h>
#include <trview.app/Messages/Messages.h>

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
    void activate_context_menu(MockPicking& picking, MockMouse& mouse, std::weak_ptr<ICameraSink> camera_sink)
    {
        PickResult pick_result{};
        pick_result.hit = true;
        pick_result.type = PickResult::Type::CameraSink;
        pick_result.position = Vector3::Zero;
        pick_result.centroid = Vector3::Zero;
        pick_result.camera_sink = camera_sink;
        picking.on_pick({}, pick_result);
        mouse.mouse_click(IMouse::Button::Right);
    }

    void activate_context_menu(MockPicking& picking, MockMouse& mouse, std::weak_ptr<ISoundSource> sound_source)
    {
        PickResult pick_result{};
        pick_result.hit = true;
        pick_result.type = PickResult::Type::SoundSource;
        pick_result.position = Vector3::Zero;
        pick_result.centroid = Vector3::Zero;
        pick_result.sound_source = sound_source;
        picking.on_pick({}, pick_result);
        mouse.mouse_click(IMouse::Button::Right);
    }

    /// Simulates a context menu activation - 
    void activate_context_menu(MockPicking& picking, MockMouse& mouse, std::weak_ptr<IItem> item)
    {
        PickResult pick_result{};
        pick_result.hit = true;
        pick_result.type = PickResult::Type::Entity;
        pick_result.position = Vector3::Zero;
        pick_result.centroid = Vector3::Zero;
        pick_result.item = item;
        picking.on_pick({}, pick_result);
        mouse.mouse_click(IMouse::Button::Right);
    }

    void activate_context_menu(MockPicking& picking, MockMouse& mouse, std::weak_ptr<ILight> light)
    {
        PickResult pick_result{};
        pick_result.hit = true;
        pick_result.type = PickResult::Type::Light;
        pick_result.position = Vector3::Zero;
        pick_result.centroid = Vector3::Zero;
        pick_result.light = light;
        picking.on_pick({}, pick_result);
        mouse.mouse_click(IMouse::Button::Right);
    }

    void activate_context_menu(MockPicking& picking, MockMouse& mouse, std::weak_ptr<IRoom> room, Vector3 position = {}, Vector3 centroid = {})
    {
        PickResult pick_result{};
        pick_result.hit = true;
        pick_result.type = PickResult::Type::Room;
        pick_result.position = position;
        pick_result.centroid = centroid;
        pick_result.room = room;
        picking.on_pick({}, pick_result);
        mouse.mouse_click(IMouse::Button::Right);
    }

    void activate_context_menu(MockPicking& picking, MockMouse& mouse, std::weak_ptr<ITrigger> trigger)
    {
        PickResult pick_result{};
        pick_result.hit = true;
        pick_result.type = PickResult::Type::Trigger;
        pick_result.position = Vector3::Zero;
        pick_result.centroid = Vector3::Zero;
        pick_result.trigger = trigger;
        picking.on_pick({}, pick_result);
        mouse.mouse_click(IMouse::Button::Right);
    }

    void activate_context_menu(MockPicking& picking, MockMouse& mouse, std::weak_ptr<IWaypoint> waypoint, int index)
    {
        PickResult pick_result{};
        pick_result.hit = true;
        pick_result.type = PickResult::Type::Waypoint;
        pick_result.position = Vector3::Zero;
        pick_result.centroid = Vector3::Zero;
        pick_result.waypoint = waypoint;
        pick_result.waypoint_index = index;
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
            std::unique_ptr<ICompass> compass{ mock_unique<MockCompass>() };
            std::unique_ptr<IMeasure> measure{ mock_unique<MockMeasure>() };
            IDeviceWindow::Source device_window_source{ [](auto&&...) { return mock_unique<MockDeviceWindow>(); } };
            std::unique_ptr<ISectorHighlight> sector_highlight{ mock_unique<MockSectorHighlight>() };
            std::shared_ptr<IClipboard> clipboard{ mock_shared<MockClipboard>() };
            std::shared_ptr<MockCamera> camera{ mock_shared<MockCamera>() };
            ISamplerState::Source sampler_source{ [](auto&&...) { return mock_shared<MockSamplerState>(); } };
            std::shared_ptr<IMessageSystem> messaging{ mock_shared<MockMessageSystem>() };

            std::unique_ptr<Viewer> build()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
                ON_CALL(*camera, idle_rotation).WillByDefault(Return(true));
                return std::make_unique<Viewer>(window, device, std::move(ui), std::move(picking), std::move(mouse), shortcuts, route,
                    std::move(compass), std::move(measure), device_window_source, std::move(sector_highlight), clipboard, camera,
                    sampler_source, messaging);
            }

            test_module& with_camera(const std::shared_ptr<MockCamera>& camera)
            {
                this->camera = camera;
                return *this;
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

            test_module& with_sector_highlight(std::unique_ptr<ISectorHighlight> sector_highlight)
            {
                this->sector_highlight = std::move(sector_highlight);
                return *this;
            }

            test_module& with_messaging(std::shared_ptr<IMessageSystem> messaging)
            {
                this->messaging = messaging;
                return *this;
            }

        };
        return test_module{};
    }
}

/// Tests that the on_hide event from the UI is observed and forwarded when the item is valid.
TEST(Viewer, ItemVisibilityRaisedForValidItem)
{
    auto item = mock_shared<MockItem>();
    EXPECT_CALL(*item, set_visible(false)).Times(1);

    auto level = mock_shared<MockLevel>();
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();

    viewer->open(level, ILevel::OpenMode::Full);

    activate_context_menu(picking, mouse, item);

    ui.on_hide();
}

/// Tests that the trigger selected event is raised when the user clicks on a trigger.
TEST(Viewer, SelectTriggerRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto messaging = mock_shared<MockMessageSystem>();

    auto level = mock_shared<MockLevel>();
    auto trigger = mock_shared<MockTrigger>()->with_number(100);
    EXPECT_CALL(*level, trigger(100)).WillRepeatedly(Return(trigger));

    std::vector<trview::Message> messages;
    EXPECT_CALL(*messaging, send_message).WillRepeatedly([&](auto&& message) { messages.push_back(message); });
    
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).with_messaging(messaging).build();
    viewer->open(level, ILevel::OpenMode::Full);

    activate_context_menu(picking, mouse, trigger);
    mouse.mouse_click(IMouse::Button::Left);

    const auto found = find_message_throw(messages, "select_trigger");
    ASSERT_EQ(messages::read_select_trigger(found)->lock(), trigger);
}

TEST(Viewer, TriggerHidden)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();

    auto level = mock_shared<MockLevel>();
    auto trigger = mock_shared<MockTrigger>()->with_number(100);
    EXPECT_CALL(*trigger, set_visible(false)).Times(1);
    EXPECT_CALL(*level, trigger(100)).WillRepeatedly(Return(trigger));

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(level, ILevel::OpenMode::Full);

    activate_context_menu(picking, mouse, trigger);

    ui.on_hide();
}

/// Tests that the waypoint selected event is raised when the user clicks on a waypoint.
TEST(Viewer, SelectWaypointRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto level = mock_shared<MockLevel>();
    auto messaging = mock_shared<MockMessageSystem>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).with_messaging(messaging).build();

    std::vector<trview::Message> messages;
    EXPECT_CALL(*messaging, send_message).WillRepeatedly([&](auto&& message) { messages.push_back(message); });

    viewer->open(level, ILevel::OpenMode::Full);

    auto route = mock_shared<MockRoute>();
    auto waypoint = mock_shared<MockWaypoint>();
    EXPECT_CALL(*route, waypoint(100)).WillRepeatedly(Return(waypoint));
    viewer->set_route(route);

    activate_context_menu(picking, mouse, waypoint, 100);
    mouse.mouse_click(IMouse::Button::Left);

    const auto found = find_message_throw(messages, "select_waypoint");
    ASSERT_EQ(messages::read_select_waypoint(found)->lock(), waypoint);
}

/// Tests that the on_remove_waypoint event from the UI is observed and forwarded.
TEST(Viewer, RemoveWaypointRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    auto waypoint = mock_shared<MockWaypoint>();

    std::optional<uint32_t> removed_waypoint;
    auto token = viewer->on_waypoint_removed += [&removed_waypoint](const auto& waypoint) { removed_waypoint = waypoint; };

    activate_context_menu(picking, mouse, waypoint, 100);

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

    auto room = mock_shared<MockRoom>()->with_number(50);
    auto level = mock_shared<MockLevel>();

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(level, ILevel::OpenMode::Full);

    std::optional<std::tuple<Vector3, Vector3, std::shared_ptr<IRoom>, IWaypoint::Type, uint32_t>> added_waypoint;
    auto token = viewer->on_waypoint_added += [&added_waypoint](const auto& position, const auto& normal, auto room, IWaypoint::Type type, uint32_t index)
    {
        added_waypoint = { position, normal, room.lock(), type, index };
    };

    activate_context_menu(picking, mouse, room, Vector3(100, 200, 300));

    ui.on_add_waypoint();

    ASSERT_TRUE(added_waypoint.has_value());
    ASSERT_EQ(std::get<0>(added_waypoint.value()), Vector3(100, 200, 300));
    ASSERT_EQ(std::get<2>(added_waypoint.value()), room);
    ASSERT_EQ(std::get<3>(added_waypoint.value()), IWaypoint::Type::Position);
    ASSERT_EQ(std::get<4>(added_waypoint.value()), 50u);
}

TEST(Viewer, AddWaypointRaisedUsesItemPosition)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();

    auto room = mock_shared<MockRoom>()->with_number(10);
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, room(10)).WillByDefault(Return(room));
    auto item = mock_shared<MockItem>()->with_room(room)->with_number(50);

    EXPECT_CALL(*level, item(50)).WillRepeatedly(Return(item));

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(level, ILevel::OpenMode::Full);

    std::optional<std::tuple<Vector3, Vector3, std::shared_ptr<IRoom>, IWaypoint::Type, uint32_t>> added_waypoint;
    auto token = viewer->on_waypoint_added += [&added_waypoint](const auto& position, const auto& normal, auto room, IWaypoint::Type type, uint32_t index)
    {
        added_waypoint = { position, normal, room.lock(), type, index };
    };

    activate_context_menu(picking, mouse, item);

    ui.on_add_waypoint();

    ASSERT_TRUE(added_waypoint.has_value());
    ASSERT_EQ(std::get<0>(added_waypoint.value()), Vector3::Zero);
    ASSERT_EQ(std::get<2>(added_waypoint.value()), room);
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
    auto room = mock_shared<MockRoom>();

    EXPECT_CALL(ui, set_show_context_menu(false));
    EXPECT_CALL(ui, set_show_context_menu(true)).Times(1);

    activate_context_menu(picking, mouse, room);
}

TEST(Viewer, OrbitEnabledWhenItemSelectedAndAutoOrbitEnabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Orbit)).Times(2);

    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Free)).Times(1);
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Orbit)).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_camera(camera).build();

    UserSettings settings;
    settings.auto_orbit = true;
    viewer->receive_message({ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });

    viewer->set_camera_mode(ICamera::Mode::Free);
    viewer->select_item(mock_shared<MockItem>());
}

TEST(Viewer, OrbitNotEnabledWhenItemSelectedAndAutoOrbitDisabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Orbit)).Times(1);

    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Free)).Times(1);
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Orbit)).Times(0);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_camera(camera).build();

    UserSettings settings;
    settings.auto_orbit = false;
    viewer->receive_message({ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });

    viewer->set_camera_mode(ICamera::Mode::Free);
    viewer->select_item({});
}

TEST(Viewer, OrbitEnabledWhenTriggerSelectedAndAutoOrbitEnabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Orbit)).Times(2);

    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Free)).Times(1);
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Orbit)).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_camera(camera).build();

    UserSettings settings;
    settings.auto_orbit = true;
    viewer->receive_message({ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });

    viewer->set_camera_mode(ICamera::Mode::Free);

    auto trigger = mock_shared<MockTrigger>();
    viewer->select_trigger(trigger);
}

TEST(Viewer, OrbitNotEnabledWhenTriggerSelectedAndAutoOrbitDisabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Orbit)).Times(1);

    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Free)).Times(1);
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Orbit)).Times(0);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_camera(camera).build();

    UserSettings settings;
    settings.auto_orbit = false;
    viewer->receive_message({ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });

    viewer->set_camera_mode(ICamera::Mode::Free);

    auto trigger = mock_shared<MockTrigger>();
    viewer->select_trigger(trigger);
}

TEST(Viewer, OrbitEnabledWhenWaypointSelectedAndAutoOrbitEnabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Orbit)).Times(2);

    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Free)).Times(1);
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Orbit)).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_camera(camera).build();

    UserSettings settings;
    settings.auto_orbit = true;
    viewer->receive_message({ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });

    viewer->set_camera_mode(ICamera::Mode::Free);

    auto mesh = mock_shared<MockMesh>();
    viewer->select_waypoint(mock_shared<MockWaypoint>());
}

TEST(Viewer, OrbitNotEnabledWhenWaypointSelectedAndAutoOrbitDisabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Orbit)).Times(1);

    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Free)).Times(1);
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Orbit)).Times(0);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_camera(camera).build();

    UserSettings settings;
    settings.auto_orbit = false;
    viewer->receive_message({ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });

    viewer->set_camera_mode(ICamera::Mode::Free);

    auto mesh = mock_shared<MockMesh>();
    viewer->select_waypoint(mock_shared<MockWaypoint>());
}


TEST(Viewer, OrbitEnabledWhenRoomSelectedAndAutoOrbitEnabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Orbit)).Times(2);

    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Free)).Times(1);
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Orbit)).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_camera(camera).build();

    UserSettings settings;
    settings.auto_orbit = true;
    viewer->receive_message({ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });

    auto room = mock_shared<MockRoom>();
    viewer->set_camera_mode(ICamera::Mode::Free);
    viewer->select_room(room);
}

TEST(Viewer, OrbitNotEnabledWhenRoomSelectedAndAutoOrbitDisabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode);
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Orbit)).Times(1);

    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Free)).Times(1);
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Orbit)).Times(0);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_camera(camera).build();

    UserSettings settings;
    settings.auto_orbit = false;
    viewer->receive_message({ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });

    auto room = mock_shared<MockRoom>();
    viewer->set_camera_mode(ICamera::Mode::Free);
    viewer->select_room(room);
}

TEST(Viewer, CameraRotationUpdated)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto camera = mock_shared<MockCamera>();
    auto viewer = register_test_module().with_camera(camera).with_ui(std::move(ui_ptr)).build();

    ASSERT_EQ(true, testing::Mock::VerifyAndClearExpectations(camera.get()));

    EXPECT_CALL(*camera, set_rotation_yaw(2.0f)).Times(1);
    EXPECT_CALL(*camera, set_rotation_pitch(1.0f)).Times(1);

    ui.on_camera_rotation(2.0f, 1.0f);
}

TEST(Viewer, SetShowBoundingBox)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto level = mock_shared<MockLevel>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    EXPECT_CALL(*level, set_show_bounding_boxes(false)).Times(1);
    EXPECT_CALL(ui, set_toggle(testing::A<const std::string&>(), testing::A<bool>())).Times(testing::AtLeast(0));
    EXPECT_CALL(ui, set_toggle(IViewer::Options::show_bounding_boxes, true)).Times(1);
    EXPECT_CALL(*level, set_show_bounding_boxes(true)).Times(1);

    viewer->open(level, ILevel::OpenMode::Full);
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
    auto room = mock_shared<MockRoom>();

    std::optional<Vector3> raised_position;
    auto token = viewer->on_waypoint_added += [&](auto&& position, auto&&...)
    {
        raised_position = position;
    };

    activate_context_menu(picking, mouse, room, Vector3(1, 2, 3), Vector3(3, 4, 5));

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
    auto room = mock_shared<MockRoom>();

    std::optional<Vector3> raised_position;
    auto token = viewer->on_waypoint_added += [&](auto&& position, auto&&...)
    {
        raised_position = position;
    };

    activate_context_menu(picking, mouse, room, Vector3(1, 2, 3), Vector3(3, 4, 5));

    ui.on_add_mid_waypoint();
    ASSERT_TRUE(raised_position.has_value());
    ASSERT_EQ(raised_position.value(), Vector3(3, 4, 5));
}

TEST(Viewer, DepthViewOptionUpdatesLevel)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, set_neighbour_depth(6)).Times(1);
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();
    viewer->open(level, ILevel::OpenMode::Full);
    ui.on_scalar_changed(IViewer::Options::depth, 6);
}

TEST(Viewer, SetShowItems)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto level = mock_shared<MockLevel>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    EXPECT_CALL(*level, set_show_items(false)).Times(1);
    EXPECT_CALL(ui, set_toggle(testing::A<const std::string&>(), testing::A<bool>())).Times(testing::AtLeast(0));
    EXPECT_CALL(ui, set_toggle(IViewer::Options::items, true)).Times(1);
    EXPECT_CALL(*level, set_show_items(true)).Times(1);

    viewer->open(level, ILevel::OpenMode::Full);
    ui.on_toggle_changed(IViewer::Options::items, true);
}

TEST(Viewer, SelectionRendered)
{
    auto device = mock_shared<MockDevice>();
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> context{ new NiceMock<MockD3D11DeviceContext>() };
    EXPECT_CALL(*device, context).WillRepeatedly(Return(context));

    auto viewer = register_test_module().with_device(device).build();

    auto texture_storage = mock_shared<MockLevelTextureStorage>();
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, texture_storage).WillByDefault(testing::Return(texture_storage));
    EXPECT_CALL(*level, render(A<const ICamera&>(), true)).Times(1);
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, render(A<const ICamera&>(), true)).Times(1);

    viewer->open(level, ILevel::OpenMode::Full);
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
    auto level = mock_shared<MockLevel>();
    ON_CALL(*level, texture_storage).WillByDefault(testing::Return(texture_storage));
    EXPECT_CALL(*level, render(A<const ICamera&>(), false)).Times(1);
    auto route = mock_shared<MockRoute>();
    EXPECT_CALL(*route, render(A<const ICamera&>(), false)).Times(1);

    viewer->set_show_selection(false);
    viewer->open(level, ILevel::OpenMode::Full);
    viewer->set_route(route);
    viewer->render();
}

TEST(Viewer, LightVisibilityRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();

    auto light = mock_shared<MockLight>()->with_number(100);
    EXPECT_CALL(*light, set_visible(false)).Times(1);

    auto level = mock_shared<MockLevel>();

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(level, ILevel::OpenMode::Full);

    activate_context_menu(picking, mouse, light);

    ui.on_hide();
}

TEST(Viewer, RoomVisibilityRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();

    auto level = mock_shared<MockLevel>();
    auto room = mock_shared<MockRoom>()->with_number(100);
    EXPECT_CALL(*room, set_visible(false)).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();
    viewer->open(level, ILevel::OpenMode::Full);

    activate_context_menu(picking, mouse, room);

    ui.on_hide();
}

TEST(Viewer, OrbitHereOrbitsWhenSettingDisabled)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Free)).Times(1);
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Orbit)).Times(2);

    auto camera = mock_shared<MockCamera>();
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Free)).Times(1);
    EXPECT_CALL(*camera, set_mode(ICamera::Mode::Orbit)).Times(1);

    UserSettings settings;
    settings.auto_orbit = false;

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_camera(camera).build();
    viewer->set_camera_mode(ICamera::Mode::Free);
    viewer->receive_message({ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(settings) });

    ui.on_orbit();
}

TEST(Viewer, ReloadLevelSyncProperties)
{
    std::set<uint32_t> groups{ 1, 2, 3 };

    auto original = mock_shared<MockLevel>();
    EXPECT_CALL(*original, alternate_mode).WillRepeatedly(Return(true));
    EXPECT_CALL(*original, neighbour_depth).WillRepeatedly(Return(6));
    EXPECT_CALL(*original, highlight_mode_enabled(ILevel::RoomHighlightMode::Highlight)).WillRepeatedly(Return(true));
    EXPECT_CALL(*original, highlight_mode_enabled(ILevel::RoomHighlightMode::Neighbours)).WillRepeatedly(Return(true));
    EXPECT_CALL(*original, alternate_group(1)).WillRepeatedly(Return(true));
    EXPECT_CALL(*original, alternate_group(2)).WillRepeatedly(Return(true));
    EXPECT_CALL(*original, alternate_group(3)).WillRepeatedly(Return(true));

    auto reloaded = mock_shared<MockLevel>();
    EXPECT_CALL(*reloaded, set_alternate_mode(true)).Times(1);
    EXPECT_CALL(*reloaded, set_neighbour_depth(6)).Times(1);
    EXPECT_CALL(*reloaded, set_highlight_mode(ILevel::RoomHighlightMode::Neighbours, true)).Times(1);
    EXPECT_CALL(*reloaded, set_highlight_mode(ILevel::RoomHighlightMode::Highlight, true)).Times(1);
    EXPECT_CALL(*reloaded, set_alternate_group(1, true)).Times(1);
    EXPECT_CALL(*reloaded, set_alternate_group(2, true)).Times(1);
    EXPECT_CALL(*reloaded, set_alternate_group(3, true)).Times(1);
    EXPECT_CALL(*reloaded, alternate_groups).WillRepeatedly(Return(groups));

    auto viewer = register_test_module().build();

    viewer->open(original, ILevel::OpenMode::Full);
    viewer->open(reloaded, ILevel::OpenMode::Reload);
}

TEST(Viewer, ReloadSetsLevelOnUi)
{
    auto original = mock_shared<MockLevel>();
    auto reloaded = mock_shared<MockLevel>();
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_level).Times(2);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    viewer->open(original, ILevel::OpenMode::Full);
    viewer->open(reloaded, ILevel::OpenMode::Reload);
}

TEST(Viewer, CopyPosition)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto clipboard = mock_shared<MockClipboard>();
    auto room = mock_shared<MockRoom>();

    EXPECT_CALL(*clipboard, write(std::wstring(L"1024, 2048, 3072"))).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_clipboard(clipboard).with_mouse(std::move(mouse_ptr)).build();

    activate_context_menu(picking, mouse, room, { 1, 2, 3 });

    ui.on_copy(trview::IContextMenu::CopyType::Position);
}

TEST(Viewer, CopyRoom)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto clipboard = mock_shared<MockClipboard>();
    auto room = mock_shared<MockRoom>()->with_number(14);

    EXPECT_CALL(*clipboard, write(std::wstring(L"14"))).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_clipboard(clipboard).with_mouse(std::move(mouse_ptr)).build();

    activate_context_menu(picking, mouse, room);

    ui.on_copy(trview::IContextMenu::CopyType::Number);
}

TEST(Viewer, SetTriggeredBy)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto clipboard = mock_shared<MockClipboard>();

    EXPECT_CALL(ui, set_triggered_by).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_clipboard(clipboard).with_mouse(std::move(mouse_ptr)).build();

    auto level = mock_shared<MockLevel>();
    viewer->open(level, ILevel::OpenMode::Full);

    activate_context_menu(picking, mouse, mock_shared<MockItem>());
}


TEST(Viewer, SetShowRooms)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto level = mock_shared<MockLevel>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    EXPECT_CALL(*level, set_show_rooms(false)).Times(1);
    EXPECT_CALL(ui, set_toggle(testing::A<const std::string&>(), testing::A<bool>())).Times(testing::AtLeast(0));
    EXPECT_CALL(ui, set_toggle(IViewer::Options::rooms, true)).Times(1);
    EXPECT_CALL(*level, set_show_rooms(true)).Times(1);

    viewer->open(level, ILevel::OpenMode::Full);
    ui.on_toggle_changed(IViewer::Options::rooms, true);
}

TEST(Viewer, GoToLaraSelectsLast)
{
    auto level = mock_shared<MockLevel>();
    auto messaging = mock_shared<MockMessageSystem>();
    auto viewer = register_test_module().with_messaging(messaging).build();

    auto item1 = mock_shared<MockItem>();
    auto item2 = mock_shared<MockItem>();
    ON_CALL(*level, items).WillByDefault(Return(std::vector<std::weak_ptr<IItem>>{ item1, item2 }));

    std::optional<trview::Message> raised;
    EXPECT_CALL(*messaging, send_message).Times(testing::AtLeast(1)).WillRepeatedly(testing::SaveArg<0>(&raised));

    viewer->open(level, ILevel::OpenMode::Full);

    ASSERT_TRUE(raised);
    ASSERT_EQ(std::static_pointer_cast<MessageData<std::weak_ptr<IItem>>>(raised->data)->value.lock(), item2);
}

TEST(Viewer, CameraSinkVisibilityRaisedForValidItem)
{
    auto cs = mock_shared<MockCameraSink>()->with_number(123);
    EXPECT_CALL(*cs, set_visible(false)).Times(1);

    auto level = mock_shared<MockLevel>();
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();

    viewer->open(level, ILevel::OpenMode::Full);

    activate_context_menu(picking, mouse, cs);

    ui.on_hide();
}

TEST(Viewer, SetShowCameraSinks)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto level = mock_shared<MockLevel>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    EXPECT_CALL(*level, set_show_camera_sinks(false)).Times(1);
    EXPECT_CALL(ui, set_toggle(testing::A<const std::string&>(), testing::A<bool>())).Times(testing::AtLeast(0));
    EXPECT_CALL(ui, set_toggle(IViewer::Options::camera_sinks, true)).Times(1);
    EXPECT_CALL(*level, set_show_camera_sinks(true)).Times(1);

    viewer->open(level, ILevel::OpenMode::Full);
    ui.on_toggle_changed(IViewer::Options::camera_sinks, true);
}

TEST(Viewer, SetTriggeredByCameraSink)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto clipboard = mock_shared<MockClipboard>();

    EXPECT_CALL(ui, set_triggered_by).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_clipboard(clipboard).with_mouse(std::move(mouse_ptr)).build();

    auto level = mock_shared<MockLevel>();
    viewer->open(level, ILevel::OpenMode::Full);

    activate_context_menu(picking, mouse, mock_shared<MockCameraSink>());
}

TEST(Viewer, SetShowLighting)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto level = mock_shared<MockLevel>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    EXPECT_CALL(*level, set_show_lighting(false)).Times(1);
    EXPECT_CALL(ui, set_toggle(testing::A<const std::string&>(), testing::A<bool>())).Times(testing::AtLeast(0));
    EXPECT_CALL(ui, set_toggle(IViewer::Options::lighting, true)).Times(1);
    EXPECT_CALL(*level, set_show_lighting(true)).Times(1);

    viewer->open(level, ILevel::OpenMode::Full);
    ui.on_toggle_changed(IViewer::Options::lighting, true);
}

TEST(Viewer, SetRouteForwarded)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_route).Times(2);
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();
    viewer->set_route(mock_shared<MockRoute>());
}

TEST(Viewer, ToggleSaved)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto messaging = mock_shared<MockMessageSystem>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_messaging(messaging).build();
    viewer->receive_message({ .type = "settings", .data = std::make_shared<MessageData<UserSettings>>(UserSettings{}) });

    trview::Message called_settings;
    EXPECT_CALL(*messaging, send_message).Times(testing::AtLeast(1)).WillRepeatedly(testing::SaveArg<0>(&called_settings));

    ui.on_toggle_changed(IViewer::Options::water, true);

    const std::unordered_map<std::string, bool> expected { { IViewer::Options::water, true } };
    ASSERT_EQ(std::static_pointer_cast<MessageData<UserSettings>>(called_settings.data)->value.toggles, expected);
}

TEST(Viewer, AlternateGroupForwarded)
{
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, set_alternate_group(6, true)).Times(1);
    ON_CALL(*level, alternate_groups).WillByDefault(Return(std::set<uint32_t>{ 6 }));

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_alternate_group(6, true)).Times(1);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();
    viewer->open(level, ILevel::OpenMode::Full);

    ui.on_alternate_group(6, true);
}

TEST(Viewer, InvalidAlternateGroupNotForwarded)
{
    auto level = mock_shared<MockLevel>();
    EXPECT_CALL(*level, set_alternate_group).Times(0);

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_alternate_group).Times(0);

    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();
    viewer->open(level, ILevel::OpenMode::Full);

    ui.on_alternate_group(6, true);
}

TEST(Viewer, FontForwarded)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    std::optional<std::pair<std::string, FontSetting>> raised;
    auto token = viewer->on_font += [&](auto&& key, auto&& value) { raised = { key, value }; };

    ui.on_font("test", { .name = "Test", .filename = "test.ttf", .size = 100 });

    ASSERT_EQ(raised.has_value(), true);
    ASSERT_EQ(raised->first, "test");
    ASSERT_EQ(raised->second.name, "Test");
    ASSERT_EQ(raised->second.filename, "test.ttf");
    ASSERT_EQ(raised->second.size, 100);
}

TEST(Viewer, CameraModeForwarded)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Orbit)).Times(1);
    EXPECT_CALL(ui, set_camera_mode(ICamera::Mode::Axis)).Times(1);

    auto camera = mock_shared<MockCamera>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_camera(camera).build();

    camera->on_mode_changed(ICamera::Mode::Axis);
}

TEST(Viewer, SoundSourceHidden)
{
    auto sound_source = mock_shared<MockSoundSource>();
    EXPECT_CALL(*sound_source, set_visible(false));

    auto level = mock_shared<MockLevel>();

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).build();

    viewer->open(level, ILevel::OpenMode::Full);

    activate_context_menu(picking, mouse, sound_source);
    ui.on_hide();
}

TEST(Viewer, SelectSoundSourceRaised)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<MockMouse>();
    auto level = mock_shared<MockLevel>();
    auto messaging = mock_shared<MockMessageSystem>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).with_picking(std::move(picking_ptr)).with_mouse(std::move(mouse_ptr)).with_messaging(messaging).build();

    viewer->open(level, ILevel::OpenMode::Full);

    std::optional<trview::Message> message;
    EXPECT_CALL(*messaging, send_message).Times(1).WillRepeatedly(testing::SaveArg<0>(&message));

    auto sound_source = mock_shared<MockSoundSource>();
    activate_context_menu(picking, mouse, sound_source);
    mouse.mouse_click(IMouse::Button::Left);

    ASSERT_TRUE(message.has_value());
    ASSERT_EQ(messages::read_select_sound_source(message.value())->lock(), sound_source);
}

TEST(Viewer, SetShowSoundSources)
{
    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto level = mock_shared<MockLevel>();
    auto viewer = register_test_module().with_ui(std::move(ui_ptr)).build();

    EXPECT_CALL(*level, set_show_sound_sources(false)).Times(1);
    EXPECT_CALL(ui, set_toggle(testing::A<const std::string&>(), testing::A<bool>())).Times(testing::AtLeast(0));
    EXPECT_CALL(ui, set_toggle(IViewer::Options::sound_sources, true)).Times(1);
    EXPECT_CALL(*level, set_show_sound_sources(true)).Times(1);

    viewer->open(level, ILevel::OpenMode::Full);
    ui.on_toggle_changed(IViewer::Options::sound_sources, true);
}

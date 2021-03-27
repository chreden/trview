#include <trview.app/Windows/Viewer.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.tests.common/Window.h>
#include <trview.app/Mocks/Elements/ILevel.h>
#include <trview.app/Mocks/Geometry/IPicking.h>
#include <trview.app/Mocks/UI/IViewerUI.h>
#include <trview.app/Mocks/Routing/IRoute.h>
#include <trview.input/Mocks/IMouse.h>
#include <trview.app/Mocks/Tools/ICompass.h>
#include <trview.graphics/mocks/ISprite.h>

using testing::NiceMock;
using testing::Return;
using namespace trview;
using namespace trview::mocks;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trview::tests;
using namespace DirectX::SimpleMath;

namespace
{
    /// Simulates a context menu activation - 
    void activate_context_menu(
        MockPicking& picking,
        input::mocks::MockMouse& mouse,
        PickResult::Type type,
        uint32_t index)
    {
        PickResult pick_result{};
        pick_result.hit = true;
        pick_result.type = type;
        pick_result.index = index;
        picking.on_pick({}, pick_result);
        mouse.mouse_click(input::IMouse::Button::Right);
    }
}

/// Tests that the on_select_item event from the UI is observed and forwarded.
TEST(Viewer, SelectItemRaisedForValidItem)
{
    auto window = create_test_window(L"ViewerTests");

    auto device = std::make_shared<MockDevice>();
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto route = std::make_shared<MockRoute>();

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [sprite_ptr_source, sprite] = create_mock<MockSprite>();
    auto sprite_ptr = std::move(sprite_ptr_source);

    Item item(123, 0, 0, L"Test", 0, 0, {}, Vector3::Zero);
    MockLevel level;

    std::vector<Item> items_list{ item };
    EXPECT_CALL(level, items)
        .WillRepeatedly([&]() { return items_list; });

    Viewer viewer(window, device, std::move(ui_ptr), std::make_unique<MockPicking>(), std::make_unique<input::mocks::MockMouse>(), shortcuts, route,
        [&](auto) { return std::move(sprite_ptr); }, std::make_unique<MockCompass>());
    viewer.open(&level);

    std::optional<Item> raised_item;
    auto token = viewer.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    ui.on_select_item(0);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 123);
}

/// Tests that the on_hide event from the UI is observed but not forwarded when the item is invalid.
TEST(Viewer, SelectItemNotRaisedForInvalidItem)
{
    auto window = create_test_window(L"ViewerTests");

    auto device = std::make_shared<MockDevice>();
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto route = std::make_shared<MockRoute>();

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [sprite_ptr_source, sprite] = create_mock<MockSprite>();
    auto sprite_ptr = std::move(sprite_ptr_source);

    Viewer viewer(window, device, std::move(ui_ptr), std::make_unique<MockPicking>(), std::make_unique<input::mocks::MockMouse>(), shortcuts, route,
        [&](auto) { return std::move(sprite_ptr); }, std::make_unique<MockCompass>());

    std::optional<Item> raised_item;
    auto token = viewer.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    ui.on_select_item(0);

    ASSERT_FALSE(raised_item.has_value());
}

/// Tests that the on_hide event from the UI is observed and forwarded when the item is valid.
TEST(Viewer, ItemVisibilityRaisedForValidItem)
{
    auto window = create_test_window(L"ViewerTests");

    auto device = std::make_shared<MockDevice>();
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto route = std::make_shared<MockRoute>();

    Item item(123, 0, 0, L"Test", 0, 0, {}, Vector3::Zero);
    MockLevel level;

    std::vector<Item> items_list{ item };
    EXPECT_CALL(level, items)
        .WillRepeatedly([&]() { return items_list; });

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [sprite_ptr_source, sprite] = create_mock<MockSprite>();
    auto sprite_ptr = std::move(sprite_ptr_source);
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<input::mocks::MockMouse>();

    Viewer viewer(window, device, std::move(ui_ptr), std::move(picking_ptr), std::move(mouse_ptr), shortcuts, route,
        [&](auto) { return std::move(sprite_ptr); }, std::make_unique<MockCompass>());
    viewer.open(&level);

    std::optional<std::tuple<Item, bool>> raised_item;
    auto token = viewer.on_item_visibility += [&raised_item](const auto& item, auto visible) { raised_item = { item, visible }; };

    activate_context_menu(picking, mouse, PickResult::Type::Entity, 0);

    ui.on_hide();

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(std::get<0>(raised_item.value()).number(), 123);
    ASSERT_FALSE(std::get<1>(raised_item.value()));
}

/// Tests that the on_settings event from the UI is observed and forwarded.
TEST(Viewer, SettingsRaised)
{
    auto window = create_test_window(L"ViewerTests");

    auto device = std::make_shared<MockDevice>();
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto route = std::make_shared<MockRoute>();

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [sprite_ptr_source, sprite] = create_mock<MockSprite>();
    auto sprite_ptr = std::move(sprite_ptr_source);
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<input::mocks::MockMouse>();

    Viewer viewer(window, device, std::move(ui_ptr), std::make_unique<MockPicking>(), std::make_unique<input::mocks::MockMouse>(), shortcuts, route,
        [&](auto) { return std::move(sprite_ptr); }, std::make_unique<MockCompass>());

    std::optional<UserSettings> raised_settings;
    auto token = viewer.on_settings += [&raised_settings](const auto& settings) { raised_settings = settings; };

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
    auto window = create_test_window(L"ViewerTests");

    auto device = std::make_shared<MockDevice>();
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto route = std::make_shared<MockRoute>();

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [sprite_ptr_source, sprite] = create_mock<MockSprite>();
    auto sprite_ptr = std::move(sprite_ptr_source);
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<input::mocks::MockMouse>();

    Viewer viewer(window, device, std::move(ui_ptr), std::make_unique<MockPicking>(), std::make_unique<input::mocks::MockMouse>(), shortcuts, route,
        [&](auto) { return std::move(sprite_ptr); }, std::make_unique<MockCompass>());

    std::optional<uint32_t> raised_room;
    auto token = viewer.on_room_selected += [&raised_room](const auto& room) { raised_room = room; };

    ui.on_select_room(0);

    ASSERT_TRUE(raised_room.has_value());
    ASSERT_EQ(raised_room.value(), 0u);
}

/// Tests that the trigger selected event is raised when the user clicks on a trigger.
TEST(Viewer, SelectTriggerRaised)
{
    auto window = create_test_window(L"ViewerTests");

    auto device = std::make_shared<MockDevice>();
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto route = std::make_shared<MockRoute>();

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [sprite_ptr_source, sprite] = create_mock<MockSprite>();
    auto sprite_ptr = std::move(sprite_ptr_source);
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<input::mocks::MockMouse>();

    MockLevel level;
    std::vector<Trigger*> triggers_list(101);
    auto trigger = std::make_unique<Trigger>(100, 10, 0, 0, TriggerInfo{});
    triggers_list[100] = trigger.get();

    EXPECT_CALL(level, triggers)
        .WillRepeatedly([&]() { return triggers_list; });

    Viewer viewer(window, device, std::move(ui_ptr), std::move(picking_ptr), std::move(mouse_ptr), shortcuts, route,
        [&](auto) { return std::move(sprite_ptr); }, std::make_unique<MockCompass>());
    viewer.open(&level);

    std::optional<Trigger*> selected_trigger;
    auto token = viewer.on_trigger_selected += [&selected_trigger](const auto& trigger) { selected_trigger = trigger; };

    activate_context_menu(picking, mouse, PickResult::Type::Trigger, 100);
    mouse.mouse_click(input::IMouse::Button::Left);

    ASSERT_TRUE(selected_trigger.has_value());
    ASSERT_EQ(selected_trigger.value(), trigger.get());
}

/// Tests that the on_hide event from the UI is observed and forwarded for triggers.
TEST(Viewer, TriggerVisibilityRaised)
{
    auto window = create_test_window(L"ViewerTests");

    auto device = std::make_shared<MockDevice>();
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto route = std::make_shared<MockRoute>();

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [sprite_ptr_source, sprite] = create_mock<MockSprite>();
    auto sprite_ptr = std::move(sprite_ptr_source);
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<input::mocks::MockMouse>();

    MockLevel level;
    std::vector<Trigger*> triggers_list(101);
    auto trigger = std::make_unique<Trigger>(100, 10, 0, 0, TriggerInfo{});
    triggers_list[100] = trigger.get();

    EXPECT_CALL(level, triggers)
        .WillRepeatedly([&]() { return triggers_list; });

    Viewer viewer(window, device, std::move(ui_ptr), std::move(picking_ptr), std::move(mouse_ptr), shortcuts, route,
        [&](auto) { return std::move(sprite_ptr); }, std::make_unique<MockCompass>());
    viewer.open(&level);

    std::optional<std::tuple<Trigger*, bool>> raised_trigger;
    auto token = viewer.on_trigger_visibility += [&raised_trigger](const auto& trigger, auto visible) { raised_trigger = { trigger, visible }; };

    activate_context_menu(picking, mouse, PickResult::Type::Trigger, 100);

    ui.on_hide();

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(std::get<0>(raised_trigger.value()), trigger.get());
    ASSERT_FALSE(std::get<1>(raised_trigger.value()));
}

/// Tests that the waypoint selected event is raised when the user clicks on a waypoint.
TEST(Viewer, SelectWaypointRaised)
{
    auto window = create_test_window(L"ViewerTests");

    auto device = std::make_shared<MockDevice>();
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto route = std::make_shared<MockRoute>();

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [sprite_ptr_source, sprite] = create_mock<MockSprite>();
    auto sprite_ptr = std::move(sprite_ptr_source);
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<input::mocks::MockMouse>();

    Viewer viewer(window, device, std::move(ui_ptr), std::move(picking_ptr), std::move(mouse_ptr), shortcuts, route,
        [&](auto) { return std::move(sprite_ptr); }, std::make_unique<MockCompass>());

    std::optional<uint32_t> selected_waypoint;
    auto token = viewer.on_waypoint_selected += [&selected_waypoint](const auto& waypoint) { selected_waypoint = waypoint; };

    activate_context_menu(picking, mouse, PickResult::Type::Waypoint, 100);
    mouse.mouse_click(input::IMouse::Button::Left);

    ASSERT_TRUE(selected_waypoint.has_value());
    ASSERT_EQ(selected_waypoint.value(), 100u);
}

/// Tests that the on_remove_waypoint event from the UI is observed and forwarded.
TEST(Viewer, RemoveWaypointRaised)
{
    auto window = create_test_window(L"ViewerTests");

    auto device = std::make_shared<MockDevice>();
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto route = std::make_shared<MockRoute>();

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [sprite_ptr_source, sprite] = create_mock<MockSprite>();
    auto sprite_ptr = std::move(sprite_ptr_source);
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<input::mocks::MockMouse>();

    Viewer viewer(window, device, std::move(ui_ptr), std::move(picking_ptr), std::move(mouse_ptr), shortcuts, route,
        [&](auto) { return std::move(sprite_ptr); }, std::make_unique<MockCompass>());

    std::optional<uint32_t> removed_waypoint;
    auto token = viewer.on_waypoint_removed += [&removed_waypoint](const auto& waypoint) { removed_waypoint = waypoint; };

    activate_context_menu(picking, mouse, PickResult::Type::Waypoint, 100);

    ui.on_remove_waypoint();

    ASSERT_TRUE(removed_waypoint.has_value());
    ASSERT_EQ(removed_waypoint.value(), 100u);
}

/// Tests that the on_add_waypoint event from the UI is observed and forwarded.
TEST(Viewer, AddWaypointRaised)
{
    auto window = create_test_window(L"ViewerTests");

    auto device = std::make_shared<MockDevice>();
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto route = std::make_shared<MockRoute>();

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [sprite_ptr_source, sprite] = create_mock<MockSprite>();
    auto sprite_ptr = std::move(sprite_ptr_source);
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<input::mocks::MockMouse>();

    MockLevel level;
    std::vector<Item> items_list(51);
    Item item(50, 10, 0, L"Test", 0, 0, {}, Vector3::Zero);
    items_list[50] = item;

    EXPECT_CALL(level, items)
        .WillRepeatedly([&]() { return items_list; });

    Viewer viewer(window, device, std::move(ui_ptr), std::move(picking_ptr), std::move(mouse_ptr), shortcuts, route,
        [&](auto) { return std::move(sprite_ptr); }, std::make_unique<MockCompass>());
    viewer.open(&level);

    std::optional<std::tuple<Vector3, uint32_t, Waypoint::Type, uint32_t>> added_waypoint;
    auto token = viewer.on_waypoint_added += [&added_waypoint](const auto& position, uint32_t room, Waypoint::Type type, uint32_t index)
    {
        added_waypoint = { position, room, type, index };
    };

    activate_context_menu(picking, mouse, PickResult::Type::Entity, 50);

    ui.on_add_waypoint();

    ASSERT_TRUE(added_waypoint.has_value());
    ASSERT_EQ(std::get<1>(added_waypoint.value()), 10u);
    ASSERT_EQ(std::get<2>(added_waypoint.value()), Waypoint::Type::Entity);
    ASSERT_EQ(std::get<3>(added_waypoint.value()), 50u);
}

/// Tests that right clicking activates the context menu.
TEST(Viewer, RightClickActivatesContextMenu)
{
    auto window = create_test_window(L"ViewerTests");

    auto device = std::make_shared<MockDevice>();
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto route = std::make_shared<MockRoute>();

    auto [ui_ptr, ui] = create_mock<MockViewerUI>();
    auto [sprite_ptr_source, sprite] = create_mock<MockSprite>();
    auto sprite_ptr = std::move(sprite_ptr_source);
    auto [picking_ptr, picking] = create_mock<MockPicking>();
    auto [mouse_ptr, mouse] = create_mock<input::mocks::MockMouse>();

    Viewer viewer(window, device, std::move(ui_ptr), std::move(picking_ptr), std::move(mouse_ptr), shortcuts, route,
        [&](auto) { return std::move(sprite_ptr); }, std::make_unique<MockCompass>());

    EXPECT_CALL(ui, set_show_context_menu(false));
    EXPECT_CALL(ui, set_show_context_menu(true)).Times(1);

    activate_context_menu(picking, mouse, PickResult::Type::Room, 0);
}
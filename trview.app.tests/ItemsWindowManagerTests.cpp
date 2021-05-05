#include <trview.app/Windows/ItemsWindowManager.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.app/Mocks/Windows/IItemsWindow.h>
#include <trview.app/Elements/Types.h>
#include <trview.app/Mocks/Geometry/IMesh.h>
#include <trview.app/Mocks/Elements/ITrigger.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;

TEST(ItemsWindowManager, AddToRouteEventRaised)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>& { return shortcut_handler; });
    auto mock_window = std::make_shared<MockItemsWindow>();
    ItemsWindowManager manager(create_test_window(L"ItemsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    std::optional<Item> raised_item;
    auto token = manager.on_add_to_route += [&raised_item](const auto& item) { raised_item = item; };

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);

    Item test_item(100, 10, 1, L"Lara", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero);
    created_window->on_add_to_route(test_item);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 100);
}

TEST(ItemsWindowManager, ItemSelectedEventRaised)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockItemsWindow>();
    ItemsWindowManager manager(create_test_window(L"ItemsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    std::optional<Item> raised_item;
    auto token = manager.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);

    Item test_item(100, 10, 1, L"Lara", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero);
    created_window->on_item_selected(test_item);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 100);
}

TEST(ItemsWindowManager, ItemVisibilityEventRaised)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockItemsWindow>();
    ItemsWindowManager manager(create_test_window(L"ItemsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    std::optional<std::tuple<Item, bool>> raised_item;
    auto token = manager.on_item_visibility += [&raised_item](const auto& item, bool state) { raised_item = { item, state }; };

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);

    Item test_item(100, 10, 1, L"Lara", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero);
    created_window->on_item_visibility(test_item, true);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(std::get<0>(raised_item.value()).number(), 100);
    ASSERT_EQ(std::get<1>(raised_item.value()), true);
}

TEST(ItemsWindowManager, TriggerSelectedEventRaised)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockItemsWindow>();
    ItemsWindowManager manager(create_test_window(L"ItemsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    std::optional<std::weak_ptr<ITrigger>> raised_trigger;
    auto token = manager.on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto test_trigger = std::make_shared<MockTrigger>();
    created_window->on_trigger_selected(test_trigger);

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value().lock(), test_trigger);
}

TEST(ItemsWindowManager, SetItemsSetsItemsOnWindows)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_items).Times(2);
    ItemsWindowManager manager(create_test_window(L"ItemsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    manager.set_items(items);
}

TEST(ItemsWindowManager, SetItemVisibilityUpdatesWindows)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, update_items).Times(1);
    ItemsWindowManager manager(create_test_window(L"ItemsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
    };
    manager.set_items(items);
    manager.set_item_visible(items[0], false);
}

TEST(ItemsWindowManager, SetTriggersSetsTriggersOnWindows)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(2);
    ItemsWindowManager manager(create_test_window(L"ItemsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto trigger = std::make_shared<MockTrigger>();
    manager.set_triggers({ trigger });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
    manager.set_triggers({ trigger });
}

TEST(ItemsWindowManager, SetRoomSetsRoomOnWindows)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_current_room).Times(2);
    ItemsWindowManager manager(create_test_window(L"ItemsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
    manager.set_room(1);
}

TEST(ItemsWindowManager, SetSelectedItemSetsSelectedItemOnWindows)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_selected_item).Times(1);
    ItemsWindowManager manager(create_test_window(L"ItemsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 1, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    manager.set_items(items);

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
    manager.set_selected_item(items[1]);
}

TEST(ItemsWindowManager, CreateWindowCreatesNewWindowWithSavedValues)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_items).Times(1);
    ItemsWindowManager manager(create_test_window(L"ItemsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 1, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };

    manager.set_items(items);

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
}

TEST(ItemsWindowManager, CreateItemsWindowKeyboardShortcut)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockItemsWindow>();
    ItemsWindowManager manager(create_test_window(L"ItemsWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });
}

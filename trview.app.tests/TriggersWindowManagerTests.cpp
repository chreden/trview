#include <trview.app/Windows/TriggersWindowManager.h>
#include <trview.app/Elements/Types.h>
#include <trview.app/Mocks/Windows/ITriggersWindow.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.app/Mocks/Elements/ITrigger.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

TEST(TriggersWindowManager, CreateTriggersWindowKeyboardShortcut)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });
}

TEST(TriggersWindowManager, CreateTriggersWindowCreatesNewWindowWithSavedValues)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(1);
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto trigger1 = std::make_shared<MockTrigger>();
    auto trigger2 = std::make_shared<MockTrigger>();
    manager.set_triggers({ trigger1, trigger2 });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
}

TEST(TriggersWindowManager, CreateTriggersWindowSetsSelectedTriggerOnWindows)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_selected_trigger).Times(1);
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto trigger1 = std::make_shared<MockTrigger>();
    auto trigger2 = std::make_shared<MockTrigger>();
    manager.set_triggers({ trigger1, trigger2 });
    manager.set_selected_trigger(trigger2);

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
}

TEST(TriggersWindowManager, ItemSelectedEventRaised)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    std::optional<Item> raised_item;
    auto token = manager.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);

    Item test_item(100, 10, 1, L"Lara", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero);
    created_window->on_item_selected(test_item);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 100);
}

TEST(TriggersWindowManager, TriggerSelectedEventRaised)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    std::optional<std::weak_ptr<ITrigger>> raised_trigger;
    auto token = manager.on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto trigger = std::make_shared<MockTrigger>();
    created_window->on_trigger_selected(trigger);

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value().lock(), trigger);
}

TEST(TriggersWindowManager, TriggerVisibilityEventRaised)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    std::optional<std::tuple<std::weak_ptr<ITrigger>, bool>> raised_trigger;
    auto token = manager.on_trigger_visibility += [&raised_trigger](const auto& trigger, bool state) { raised_trigger = { trigger, state }; };

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto trigger = std::make_shared<MockTrigger>();
    created_window->on_trigger_visibility(trigger, true);

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(std::get<0>(raised_trigger.value()).lock(), trigger);
    ASSERT_EQ(std::get<1>(raised_trigger.value()), true);
}

TEST(TriggersWindowManager, AddToRouteEventRaised)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    std::optional<std::weak_ptr<ITrigger>> raised_trigger;
    auto token = manager.on_add_to_route += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto trigger = std::make_shared<MockTrigger>();
    created_window->on_add_to_route(trigger);

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value().lock(), trigger);
}

TEST(TriggersWindowManager, SetItemsSetsItemsOnWindows)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_items).Times(2);
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Test Object", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
    };
    manager.set_items(items);
}

TEST(TriggersWindowManager, SetTriggersSetsTriggersOnWindows)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(2);
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger1 = std::make_shared<MockTrigger>();
    auto trigger2 = std::make_shared<MockTrigger>();
    manager.set_triggers({ trigger1, trigger2 });
}

TEST(TriggersWindowManager, SetTriggersClearsSelectedTrigger)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(3);
    EXPECT_CALL(*mock_window, clear_selected_trigger).Times(2);
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger = std::make_shared<MockTrigger>();
    manager.set_triggers({ trigger });

    ASSERT_EQ(manager.selected_trigger().lock(), nullptr);
    manager.set_selected_trigger(trigger);
    ASSERT_EQ(manager.selected_trigger().lock(), trigger);
    manager.set_triggers({});
    ASSERT_EQ(manager.selected_trigger().lock(), nullptr);
}

TEST(TriggersWindowManager, SetTriggerVisibilityUpdatesWindows)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, update_triggers).Times(1);
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger = std::make_shared<MockTrigger>();
    manager.set_triggers({ trigger });
    manager.set_trigger_visible(trigger, false);
}

TEST(TriggersWindowManager, SetRoomSetsRoomOnWindows)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_current_room).Times(2);
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
    manager.set_room(1);
}

TEST(TriggersWindowManager, SetSelectedTriggerSetsSelectedTriggerOnWindows)
{
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_selected_trigger).Times(2);
    TriggersWindowManager manager(create_test_window(L"TriggersWindowManagerTests"), shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger1 = std::make_shared<MockTrigger>();
    auto trigger2 = std::make_shared<MockTrigger>();
    manager.set_triggers({ trigger1, trigger2 });
    manager.set_selected_trigger(trigger2);
}

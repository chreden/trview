#include <trview.app/Windows/TriggersWindowManager.h>
#include <trview.app/Elements/Types.h>
#include <trview.app/Mocks/Windows/ITriggersWindow.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/IRoom.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"TriggersWindowManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            TriggersWindow::Source window_source{ [](auto&&...) { return mock_shared<MockTriggersWindow>(); } };

            test_module& with_window_source(const TriggersWindow::Source& source)
            {
                this->window_source = source;
                return *this;
            }

            test_module& with_shortcuts(const std::shared_ptr<MockShortcuts>& shortcuts)
            {
                this->shortcuts = shortcuts;
                return *this;
            }

            test_module()
            {
                EXPECT_CALL(*shortcuts, add_shortcut).WillRepeatedly([&](auto, auto) -> Event<>&{ return shortcut_handler; });
            }

            std::unique_ptr<TriggersWindowManager> build()
            {
                return std::make_unique<TriggersWindowManager>(window, shortcuts, window_source);
            }
        };

        return test_module{};
    }
}

TEST(TriggersWindowManager, CreateTriggersWindowKeyboardShortcut)
{
    auto shortcuts = mock_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto manager = register_test_module().with_shortcuts(shortcuts).build();
}

TEST(TriggersWindowManager, CreateTriggersWindowCreatesNewWindowWithSavedValues)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto trigger1 = mock_shared<MockTrigger>();
    auto trigger2 = mock_shared<MockTrigger>();
    manager->set_triggers({ trigger1, trigger2 });

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
}

TEST(TriggersWindowManager, CreateTriggersWindowSetsSelectedTriggerOnWindows)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_selected_trigger).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto trigger1 = mock_shared<MockTrigger>();
    auto trigger2 = mock_shared<MockTrigger>();
    manager->set_triggers({ trigger1, trigger2 });
    manager->set_selected_trigger(trigger2);

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
}

TEST(TriggersWindowManager, ItemSelectedEventRaised)
{
    auto manager = register_test_module().build();

    std::shared_ptr<IItem> raised_item;
    auto token = manager->on_item_selected += [&raised_item](const auto& item) { raised_item = item.lock(); };

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto test_item = mock_shared<MockItem>();
    created_window->on_item_selected(test_item);

    ASSERT_TRUE(raised_item);
    ASSERT_EQ(raised_item, test_item);
}

TEST(TriggersWindowManager, TriggerSelectedEventRaised)
{
    auto manager = register_test_module().build();

    std::optional<std::weak_ptr<ITrigger>> raised_trigger;
    auto token = manager->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto trigger = mock_shared<MockTrigger>();
    created_window->on_trigger_selected(trigger);

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value().lock(), trigger);
}

TEST(TriggersWindowManager, AddToRouteEventRaised)
{
    auto manager = register_test_module().build();

    std::optional<std::weak_ptr<ITrigger>> raised_trigger;
    auto token = manager->on_add_to_route += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto trigger = mock_shared<MockTrigger>();
    created_window->on_add_to_route(trigger);

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value().lock(), trigger);
}

TEST(TriggersWindowManager, SetItemsSetsItemsOnWindows)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_items).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    manager->set_items({});
}

TEST(TriggersWindowManager, SetTriggersSetsTriggersOnWindows)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger1 = mock_shared<MockTrigger>();
    auto trigger2 = mock_shared<MockTrigger>();
    manager->set_triggers({ trigger1, trigger2 });
}

TEST(TriggersWindowManager, SetTriggersClearsSelectedTrigger)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(3);
    EXPECT_CALL(*mock_window, clear_selected_trigger).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger = mock_shared<MockTrigger>();
    manager->set_triggers({ trigger });

    ASSERT_EQ(manager->selected_trigger().lock(), nullptr);
    manager->set_selected_trigger(trigger);
    ASSERT_EQ(manager->selected_trigger().lock(), trigger);
    manager->set_triggers({});
    ASSERT_EQ(manager->selected_trigger().lock(), nullptr);
}

TEST(TriggersWindowManager, SetRoomSetsRoomOnWindows)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_current_room).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
    manager->set_room(mock_shared<MockRoom>());
}

TEST(TriggersWindowManager, SetSelectedTriggerSetsSelectedTriggerOnWindows)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_selected_trigger).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger1 = mock_shared<MockTrigger>();
    auto trigger2 = mock_shared<MockTrigger>();
    manager->set_triggers({ trigger1, trigger2 });
    manager->set_selected_trigger(trigger2);
}

TEST(TriggersWindowManager, WindowsUpdated)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, update(1.0f)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->update(1.0f);
}

TEST(TriggersWindowManager, CameraSinkSelectedRaised)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    std::optional<uint32_t> raised;
    auto token = manager->on_camera_sink_selected += [&](auto index)
    {
        raised = index;
    };

    manager->create_window();

    mock_window->on_camera_sink_selected(100);

    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), 100u);
}


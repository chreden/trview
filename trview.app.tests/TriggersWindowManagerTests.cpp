#include <trview.app/Windows/TriggersWindowManager.h>
#include <trview.app/Elements/Types.h>
#include <trview.app/Mocks/Windows/ITriggersWindow.h>
#include <trview.app/Windows/TriggersWindow.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/IRoom.h>
#include <trview.app/Mocks/Elements/ICameraSink.h>

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
    EXPECT_CALL(*mock_window, add_level).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    manager->add_level(mock_shared<MockLevel>());

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
}

TEST(TriggersWindowManager, CreateTriggersWindowSetsSelectedTriggerOnWindows)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_selected_trigger).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto level = mock_shared<MockLevel>();
    manager->add_level(level);

    auto trigger2 = mock_shared<MockTrigger>()->with_level(level);
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

TEST(TriggersWindowManager, SceneChangedEventRaised)
{
    auto manager = register_test_module().build();

    bool raised = false;
    auto token = manager->on_scene_changed += [&raised]() { raised = true; };

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);

    created_window->on_scene_changed();
    ASSERT_TRUE(raised);
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

TEST(TriggersWindowManager, AddLevelAddsToWindows)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, add_level).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto level1 = mock_shared<MockLevel>();
    auto level2 = mock_shared<MockLevel>();

    manager->add_level(level1);

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    manager->add_level(level2);
}

TEST(TriggersWindowManager, SetRoomSetsRoomOnWindows)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_current_room).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto level = mock_shared<MockLevel>();
    manager->add_level(level);

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
    manager->set_room(mock_shared<MockRoom>()->with_level(level));
}

TEST(TriggersWindowManager, SetSelectedTriggerSetsSelectedTriggerOnWindows)
{
    auto mock_window = mock_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_selected_trigger).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto level = mock_shared<MockLevel>();
    manager->add_level(level);

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger2 = mock_shared<MockTrigger>();
    manager->set_selected_trigger(trigger2->with_level(level));
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

    std::optional<std::shared_ptr<ICameraSink>> raised;
    auto token = manager->on_camera_sink_selected += [&](auto cam)
    {
        raised = cam.lock();
    };

    manager->create_window();

    auto camera_sink = mock_shared<MockCameraSink>();
    mock_window->on_camera_sink_selected(camera_sink);

    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), camera_sink);
}


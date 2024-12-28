#include <trview.app/Windows/ItemsWindowManager.h>
#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.app/Mocks/Windows/IItemsWindow.h>
#include <trview.app/Elements/Types.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.app/Mocks/Elements/IRoom.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::mocks;
using testing::A;

namespace
{
    Event<> shortcut_handler;

    auto register_test_module()
    {
        struct test_module
        {
            Window window{ create_test_window(L"ItemsWindowManagerTests") };
            std::shared_ptr<MockShortcuts> shortcuts{ mock_shared<MockShortcuts>() };
            ItemsWindow::Source window_source{ [](auto&&...) { return mock_shared<MockItemsWindow>(); } };

            test_module& with_window_source(const ItemsWindow::Source& source)
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

            std::unique_ptr<ItemsWindowManager> build()
            {
                return std::make_unique<ItemsWindowManager>(window, shortcuts, window_source);
            }
        };

        return test_module{};
    }
}

TEST(ItemsWindowManager, AddToRouteEventRaised)
{
    auto manager = register_test_module().build();

    std::shared_ptr<IItem> raised_item;
    auto token = manager->on_add_to_route += [&raised_item](const auto& item) { raised_item = item.lock(); };

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto test_item = mock_shared<MockItem>();
    created_window->on_add_to_route(test_item);

    ASSERT_TRUE(raised_item);
    ASSERT_EQ(raised_item, test_item);
}

TEST(ItemsWindowManager, ItemSelectedEventRaised)
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

TEST(ItemsWindowManager, SceneChangedRaised)
{
    auto manager = register_test_module().build();

    bool raised = false;
    auto token = manager->on_scene_changed += [&raised]() { raised = true; };

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto test_item = mock_shared<MockItem>();
    created_window->on_scene_changed();

    ASSERT_TRUE(raised);
}

TEST(ItemsWindowManager, TriggerSelectedEventRaised)
{
    auto manager = register_test_module().build();

    std::optional<std::weak_ptr<ITrigger>> raised_trigger;
    auto token = manager->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto test_trigger = mock_shared<MockTrigger>();
    created_window->on_trigger_selected(test_trigger);

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value().lock(), test_trigger);
}

TEST(ItemsWindowManager, SetItemsSetsItemsOnWindows)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_items).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    manager->set_items({});
}

TEST(ItemsWindowManager, SetTriggersSetsTriggersOnWindows)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto trigger = mock_shared<MockTrigger>();
    manager->set_triggers({ trigger });

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
    manager->set_triggers({ trigger });
}

TEST(ItemsWindowManager, SetRoomSetsRoomOnWindows)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_current_room).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
    manager->set_room(mock_shared<MockRoom>());
}

TEST(ItemsWindowManager, SetSelectedItemSetsSelectedItemOnWindows)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_selected_item).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
    manager->set_selected_item(mock_shared<MockItem>());
}

TEST(ItemsWindowManager, CreateWindowCreatesNewWindowWithSavedValues)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_items).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();

    auto created_window = manager->create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
}

TEST(ItemsWindowManager, CreateItemsWindowKeyboardShortcut)
{
    auto shortcuts = mock_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto manager = register_test_module().with_shortcuts(shortcuts).build();
}

TEST(ItemsWindowManager, WindowsUpdated)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, update(1.0f)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->update(1.0f);
}

TEST(ItemsWindowManager, CreateWindowPassesLevelVersion)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_level_version(trlevel::LevelVersion::Tomb4)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->set_level_version(trlevel::LevelVersion::Tomb4);
    manager->create_window();
}

TEST(ItemsWindowManager, SetLevelVersionUpdatesWindows)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_level_version(trlevel::LevelVersion::Unknown)).Times(1);
    EXPECT_CALL(*mock_window, set_level_version(trlevel::LevelVersion::Tomb4)).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->set_level_version(trlevel::LevelVersion::Tomb4);
}

TEST(ItemsWindowManager, CreateWindowPassesModelChecker)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_model_checker(A<const std::function<bool (uint32_t)>&>())).Times(1);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->set_model_checker([](auto) {return true; });
    manager->create_window();
}

TEST(ItemsWindowManager, SetModelCheckerUpdatesWindows)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_model_checker(A<const std::function<bool(uint32_t)>&>())).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->set_model_checker([](auto) {return true; });
}

TEST(ItemsWindowManager, SetNgPlusPassedToWindows)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    EXPECT_CALL(*mock_window, set_ng_plus).Times(2);
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->set_ng_plus(false);
}

TEST(ItemsWindowManager, Windows)
{
    auto mock_window = mock_shared<MockItemsWindow>();
    auto manager = register_test_module().with_window_source([&](auto&&...) { return mock_window; }).build();
    manager->create_window();
    manager->create_window();
    const auto windows = manager->windows();
    ASSERT_EQ(windows.size(), 2);
    ASSERT_EQ(windows[0].lock(), mock_window);
    ASSERT_EQ(windows[1].lock(), mock_window);
}

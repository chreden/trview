#include <trview.app/Windows/TriggersWindowManager.h>

#include <trview.common/Mocks/Windows/IShortcuts.h>
#include <trview.graphics/mocks/IDevice.h>
#include <trview.graphics/mocks/IShaderStorage.h>
#include <trview.graphics/mocks/IFontFactory.h>
#include <trview.graphics/mocks/IFont.h>
#include <trview.app/Elements/Types.h>

#include <trview.app/Mocks/Windows/ITriggersWindow.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::tests;
using namespace trview::graphics;
using namespace trview::graphics::mocks;

TEST(TriggersWindowManager, CreateTriggersWindowKeyboardShortcut)
{
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });
}

TEST(TriggersWindowManager, CreateTriggersWindowCreatesNewWindowWithSavedValues)
{
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).Times(1).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(1);
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });

    auto trigger1 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    auto trigger2 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    manager.set_triggers({ trigger1.get(), trigger2.get() });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
}

TEST(TriggersWindowManager, CreateTriggersWindowSetsSelectedTriggerOnWindows)
{
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_selected_trigger).Times(1);
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{});
    auto trigger2 = std::make_unique<Trigger>(1, 1, 100, 200, TriggerInfo{});
    manager.set_triggers({ trigger1.get(), trigger2.get() });
    manager.set_selected_trigger(trigger2.get());

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
}

TEST(TriggersWindowManager, ItemSelectedEventRaised)
{
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });

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
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });

    std::optional<const Trigger*> raised_trigger;
    auto token = manager.on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto trigger = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    created_window->on_trigger_selected(trigger.get());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value()->number(), 100);
}

TEST(TriggersWindowManager, TriggerVisibilityEventRaised)
{
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });

    std::optional<std::tuple<const Trigger*, bool>> raised_trigger;
    auto token = manager.on_trigger_visibility += [&raised_trigger](const auto& trigger, bool state) { raised_trigger = { trigger, state }; };

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto trigger = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    created_window->on_trigger_visibility(trigger.get(), true);

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(std::get<0>(raised_trigger.value())->number(), 100);
    ASSERT_EQ(std::get<1>(raised_trigger.value()), true);
}

TEST(TriggersWindowManager, AddToRouteEventRaised)
{
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });

    std::optional<const Trigger*> raised_trigger;
    auto token = manager.on_add_to_route += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);

    auto trigger = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    created_window->on_add_to_route(trigger.get());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value()->number(), 100);
}

TEST(TriggersWindowManager, SetItemsSetsItemsOnWindows)
{
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_items).Times(2);
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });

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
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_triggers).Times(2);
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger1 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    auto trigger2 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    manager.set_triggers({ trigger1.get(), trigger2.get() });
}

TEST(TriggersWindowManager, SetTriggerVisibilityUpdatesWindows)
{
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, update_triggers).Times(1);
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger1 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    manager.set_triggers({ trigger1.get() });
    manager.set_trigger_visible(trigger1.get(), false);
}

TEST(TriggersWindowManager, SetRoomSetsRoomOnWindows)
{
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_current_room).Times(2);
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);
    manager.set_room(1);
}

TEST(TriggersWindowManager, SetSelectedTriggerSetsSelectedTriggerOnWindows)
{
    auto font_factory = std::make_shared<MockFontFactory>();
    EXPECT_CALL(*font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<MockFont>(); });

    auto shader_storage = std::make_shared<MockShaderStorage>();
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Event<> shortcut_handler;
    auto shortcuts = std::make_shared<MockShortcuts>();
    EXPECT_CALL(*shortcuts, add_shortcut).WillOnce([&](auto, auto) -> Event<>&{ return shortcut_handler; });
    auto mock_window = std::make_shared<MockTriggersWindow>();
    EXPECT_CALL(*mock_window, set_selected_trigger).Times(1);
    TriggersWindowManager manager(std::make_shared<MockDevice>(), shader_storage, font_factory, test_window, shortcuts, [&mock_window](...) { return mock_window; });

    auto created_window = manager.create_window().lock();
    ASSERT_NE(created_window, nullptr);
    ASSERT_EQ(created_window, mock_window);

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{});
    auto trigger2 = std::make_unique<Trigger>(1, 1, 100, 200, TriggerInfo{});
    manager.set_triggers({ trigger1.get(), trigger2.get() });
    manager.set_selected_trigger(trigger2.get());
}

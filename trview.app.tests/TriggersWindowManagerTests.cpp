#include <trview.app/Windows/TriggersWindowManager.h>

#include <trview.graphics/Device.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/mocks/IFontFactory.h>
#include <trview.graphics/mocks/IFont.h>
#include <trview.app/Elements/Types.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::graphics;

TEST(TriggersWindowManager, CreateTriggersWindowKeyboardShortcut)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"TriggersWindowManagerTests");

    Shortcuts shortcuts(test_window);
    ASSERT_TRUE(shortcuts.shortcuts().empty());
    TriggersWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);
    ASSERT_FALSE(shortcuts.shortcuts().empty());
}

TEST(TriggersWindowManager, CreateTriggersWindowCreatesNewWindowWithSavedValues)
{
    FAIL();
}

TEST(TriggersWindowManager, ItemSelectedEventRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Shortcuts shortcuts(test_window);
    TriggersWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    std::optional<Item> raised_item;
    auto token = manager.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    Item test_item(100, 10, 1, L"Lara", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero);
    created_window->on_item_selected(test_item);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 100);
}

TEST(TriggersWindowManager, TriggerSelectedEventRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Shortcuts shortcuts(test_window);
    TriggersWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    std::optional<const Trigger*> raised_trigger;
    auto token = manager.on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    auto trigger = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    created_window->on_trigger_selected(trigger.get());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value()->number(), 100);
}

TEST(TriggersWindowManager, TriggerVisibilityEventRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Shortcuts shortcuts(test_window);
    TriggersWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    std::optional<std::tuple<const Trigger*, bool>> raised_trigger;
    auto token = manager.on_trigger_visibility += [&raised_trigger](const auto& trigger, bool state) { raised_trigger = { trigger, state }; };

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    auto trigger = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    created_window->on_trigger_visibility(trigger.get(), true);

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(std::get<0>(raised_trigger.value())->number(), 100);
    ASSERT_EQ(std::get<1>(raised_trigger.value()), true);
}

TEST(TriggersWindowManager, AddToRouteEventRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Shortcuts shortcuts(test_window);
    TriggersWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    std::optional<const Trigger*> raised_trigger;
    auto token = manager.on_add_to_route += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    auto trigger = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    created_window->on_add_to_route(trigger.get());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value()->number(), 100);
}

TEST(TriggersWindowManager, SetItemsSetsItemsOnWindows)
{
    FAIL();
}

TEST(TriggersWindowManager, SetTriggersSetsTriggersOnWindows)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Shortcuts shortcuts(test_window);
    TriggersWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    auto list = created_window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);
    ASSERT_TRUE(list->items().empty());

    auto trigger1 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    auto trigger2 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    manager.set_triggers({ trigger1.get(), trigger2.get() });

    ASSERT_EQ(list->items().size(), 2);
}

TEST(TriggersWindowManager, SetTriggerVisibilityUpdatesWindows)
{
    FAIL();
}

TEST(TriggersWindowManager, SetRoomSetsRoomOnWindows)
{
    FAIL();
}

TEST(TriggersWindowManager, SetSelectedTriggerSetsSelectedTriggerOnWindows)
{
    FAIL();
}

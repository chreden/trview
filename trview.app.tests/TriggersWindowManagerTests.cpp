#include <trview.app/Windows/TriggersWindowManager.h>

#include <trview.graphics/Device.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/mocks/IFontFactory.h>
#include <trview.graphics/mocks/IFont.h>
#include <trview.app/Elements/Types.h>
#include <trview.ui/Checkbox.h>

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
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Shortcuts shortcuts(test_window);
    TriggersWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    auto trigger1 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    auto trigger2 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{});
    manager.set_triggers({ trigger1.get(), trigger2.get() });

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    auto list = created_window->root_control()->find<ui::Listbox>(TriggersWindow::Names::triggers_listbox);
    ASSERT_NE(list, nullptr);
    ASSERT_EQ(list->items().size(), 2);
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

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Test Object", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
    };
    manager.set_items(items);
    auto trigger1 = std::make_unique<Trigger>(100, 55, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, { { TriggerCommandType::Object, 0 }} });
    manager.set_triggers({ trigger1.get() });

    manager.set_selected_trigger(trigger1.get());

    auto commands_list = created_window->root_control()->find<ui::Listbox>(TriggersWindow::Names::trigger_commands_listbox);
    ASSERT_NE(commands_list, nullptr);

    auto command_items = commands_list->items();
    ASSERT_NE(command_items.size(), 0);
    ASSERT_EQ(command_items[0].value(L"Index"), L"0");
    ASSERT_EQ(command_items[0].value(L"Entity"), L"Test Object");
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
    manager.set_triggers({ trigger1.get() });
    ASSERT_EQ(list->items().size(), 1);
    ASSERT_EQ(list->items()[0].value(L"Hide"), L"0");

    trigger1->set_visible(false);
    manager.set_trigger_visible(trigger1.get(), false);
    ASSERT_EQ(list->items()[0].value(L"Hide"), L"1");
}

TEST(TriggersWindowManager, SetRoomSetsRoomOnWindows)
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

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{});
    auto trigger2 = std::make_unique<Trigger>(1, 1, 100, 200, TriggerInfo{});
    manager.set_triggers({ trigger1.get(), trigger2.get() });
    ASSERT_EQ(list->items().size(), 2);

    auto track = created_window->root_control()->find<ui::Checkbox>(TriggersWindow::Names::track_room_checkbox);
    ASSERT_NE(track, nullptr);
    track->clicked(Point());

    ASSERT_EQ(list->items().size(), 1);
    ASSERT_EQ(list->items()[0].value(L"#"), L"0");

    manager.set_room(1);
    ASSERT_EQ(list->items().size(), 1);
    ASSERT_EQ(list->items()[0].value(L"#"), L"1");
}

TEST(TriggersWindowManager, SetSelectedTriggerSetsSelectedTriggerOnWindows)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"TriggersWindowManagerTests");
    Shortcuts shortcuts(test_window);
    TriggersWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    auto trigger1 = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{});
    auto trigger2 = std::make_unique<Trigger>(1, 1, 100, 200, TriggerInfo{});
    manager.set_triggers({ trigger1.get(), trigger2.get() });

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);
    ASSERT_FALSE(created_window->selected_trigger().has_value());

    manager.set_selected_trigger(trigger2.get());
    ASSERT_TRUE(created_window->selected_trigger().has_value());
    ASSERT_EQ(created_window->selected_trigger().value()->number(), 1);
}

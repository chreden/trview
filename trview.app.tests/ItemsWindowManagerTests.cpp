#include <trview.app/Windows/ItemsWindowManager.h>

#include <trview.graphics/Device.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/mocks/IFontFactory.h>
#include <trview.graphics/mocks/IFont.h>
#include <trview.app/Elements/Types.h>
#include <trview.ui/Checkbox.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::graphics;

TEST(ItemsWindowManager, AddToRouteEventRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"ItemsWindowManagerTests");
    Shortcuts shortcuts(test_window);
    ItemsWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    std::optional<Item> raised_item;
    auto token = manager.on_add_to_route += [&raised_item](const auto& item) { raised_item = item; };

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    Item test_item(100, 10, 1, L"Lara", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero);
    created_window->on_add_to_route(test_item);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 100);
}

TEST(ItemsWindowManager, ItemSelectedEventRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"ItemsWindowManagerTests");
    Shortcuts shortcuts(test_window);
    ItemsWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    std::optional<Item> raised_item;
    auto token = manager.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    Item test_item(100, 10, 1, L"Lara", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero);
    created_window->on_item_selected(test_item);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 100);
}

TEST(ItemsWindowManager, ItemVisibilityEventRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"ItemsWindowManagerTests");
    Shortcuts shortcuts(test_window);
    ItemsWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    std::optional<std::tuple<Item, bool>> raised_item;
    auto token = manager.on_item_visibility += [&raised_item](const auto& item, bool state) { raised_item = { item, state }; };

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    Item test_item(100, 10, 1, L"Lara", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero);
    created_window->on_item_visibility(test_item, true);

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(std::get<0>(raised_item.value()).number(), 100);
    ASSERT_EQ(std::get<1>(raised_item.value()), true);
}

TEST(ItemsWindowManager, TriggerSelectedEventRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"ItemsWindowManagerTests");
    Shortcuts shortcuts(test_window);
    ItemsWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    std::optional<Trigger*> raised_trigger;
    auto token = manager.on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    auto test_trigger = std::make_unique<Trigger>(100, 10, 1, 2, TriggerInfo{ 0,0,0, TriggerType::Antipad, 0, {} });
    created_window->on_trigger_selected(test_trigger.get());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value(), test_trigger.get());
    ASSERT_EQ(raised_trigger.value()->number(), 100);
}

TEST(ItemsWindowManager, SetItemsSetsItemsOnWindows)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"ItemsWindowManagerTests");
    Shortcuts shortcuts(test_window);
    ItemsWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    auto list = created_window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);
    ASSERT_TRUE(list->items().empty());

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };

    manager.set_items(items);
    ASSERT_EQ(list->items().size(), 2);
}

TEST(ItemsWindowManager, SetItemVisibilityUpdatesWindows)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"ItemsWindowManagerTests");
    Shortcuts shortcuts(test_window);
    ItemsWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    auto list = created_window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);
    ASSERT_TRUE(list->items().empty());

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
    };

    manager.set_items(items);
    ASSERT_EQ(list->items().size(), 1);
    ASSERT_EQ(list->items()[0].value(L"Hide"), L"0");

    manager.set_item_visible(items[0], false);
    ASSERT_EQ(list->items()[0].value(L"Hide"), L"1");
}

TEST(ItemsWindowManager, SetTriggersSetsTriggersOnWindows)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"ItemsWindowManagerTests");
    Shortcuts shortcuts(test_window);
    ItemsWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    auto trigger = std::make_unique<Trigger>(0, 0, 100, 200, TriggerInfo{ 0, 0, 0, TriggerType::Trigger, 0, {} });
    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, { trigger.get() }, DirectX::SimpleMath::Vector3::Zero)
    };
    manager.set_triggers({ trigger.get() });
    manager.set_items(items);
    manager.set_selected_item(items[1]);

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    auto triggers_list = created_window->root_control()->find<ui::Listbox>(ItemsWindow::Names::triggers_listbox);
    ASSERT_NE(triggers_list, nullptr);

    ASSERT_EQ(triggers_list->items().size(), 1);
    ASSERT_EQ(triggers_list->items()[0].value(L"#"), L"0");
}

TEST(ItemsWindowManager, SetRoomSetsRoomOnWindows)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"ItemsWindowManagerTests");
    Shortcuts shortcuts(test_window);
    ItemsWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    auto list = created_window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);
    ASSERT_TRUE(list->items().empty());

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 1, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };

    manager.set_items(items);
    ASSERT_EQ(list->items().size(), 2);

    auto track = created_window->root_control()->find<ui::Checkbox>(ItemsWindow::Names::track_room_checkbox);
    ASSERT_NE(track, nullptr);
    track->clicked(Point());

    ASSERT_EQ(list->items().size(), 1);
    ASSERT_EQ(list->items()[0].value(L"#"), L"0");

    manager.set_room(1);
    ASSERT_EQ(list->items().size(), 1);
    ASSERT_EQ(list->items()[0].value(L"#"), L"1");
}

TEST(ItemsWindowManager, SetSelectedItemSetsSelectedItemOnWindows)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"ItemsWindowManagerTests");
    Shortcuts shortcuts(test_window);
    ItemsWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 1, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    manager.set_items(items);

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);
    ASSERT_FALSE(created_window->selected_item().has_value());

    manager.set_selected_item(items[1]);
    ASSERT_TRUE(created_window->selected_item().has_value());
    ASSERT_EQ(created_window->selected_item().value().number(), 1);
}

TEST(ItemsWindowManager, CreateWindowCreatesNewWindowWithSavedValues)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"ItemsWindowManagerTests");
    Shortcuts shortcuts(test_window);
    ItemsWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 1, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };

    manager.set_items(items);

    auto created_window = manager.create_window();
    ASSERT_NE(created_window, nullptr);

    auto list = created_window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);
    ASSERT_EQ(list->items().size(), 2);
}

TEST(ItemsWindowManager, CreateItemsWindowKeyboardShortcut)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    auto test_window = create_test_window(L"ItemsWindowManagerTests");

    Shortcuts shortcuts(test_window);
    ASSERT_TRUE(shortcuts.shortcuts().empty());
    ItemsWindowManager manager(device, shader_storage, font_factory, test_window, shortcuts);
    ASSERT_FALSE(shortcuts.shortcuts().empty());
}

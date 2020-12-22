#include <trview.app/Windows/ItemsWindowManager.h>

#include <trview.graphics/Device.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/mocks/IFontFactory.h>
#include <trview.graphics/mocks/IFont.h>
#include <trview.app/Elements/Types.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::graphics;

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
    FAIL();
}

TEST(ItemsWindowManager, SetItemVisibilityUpdatesWindows)
{
    FAIL();
}

TEST(ItemsWindowManager, SetTriggersSetsTriggersOnWindows)
{
    FAIL();
}

TEST(ItemsWindowManager, SetRoomSetsRoomOnWindows)
{
    FAIL();
}

TEST(ItemsWindowManager, SetSelectedItemSetsSelectedItemOnWindows)
{
    FAIL();
}

TEST(ItemsWindowManager, CreateWindowCreatesNewWindowWithSavedValues)
{
    FAIL();
}

TEST(ItemsWindowManager, CreateItemsWindowKeyboardShortcut)
{
    FAIL();
}

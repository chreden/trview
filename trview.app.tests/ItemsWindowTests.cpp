#include <trview.app/Windows/ItemsWindow.h>
#include <trview.tests.common/Window.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.ui/Button.h>
#include <trview.ui/Checkbox.h>
#include <trview.graphics/IFont.h>
#include <trview.common/Size.h>
#include <trview.graphics/mocks/IFontFactory.h>
#include <trview.graphics/mocks/IFont.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::graphics;
using testing::NiceMock;
using testing::Return;

TEST(ItemsWindow, AddToRouteEventRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    ItemsWindow window(device, shader_storage, font_factory, create_test_window(L"ItemsWindowTests"));

    std::optional<Item> raised_item;
    auto token = window.on_add_to_route += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window.set_items(items);
    window.set_selected_item(items[1]);

    auto button = window.root_control()->find<ui::Button>(ItemsWindow::Names::add_to_route_button);
    ASSERT_NE(button, nullptr);

    button->clicked(Point());
    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 1);
}

TEST(ItemsWindow, ClearSelectedItemClearsSelection)
{
    FAIL();
}

TEST(ItemsWindow, ItemSelectedNotRaisedWhenSyncItemDisabled)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    ItemsWindow window(device, shader_storage, font_factory, create_test_window(L"ItemsWindowTests"));

    std::optional<Item> raised_item;
    auto token = window.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window.set_items(items);

    auto sync = window.root_control()->find<ui::Checkbox>(ItemsWindow::Names::sync_item_checkbox);
    ASSERT_NE(sync, nullptr);
    sync->clicked(Point());

    auto list = window.root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(row, nullptr);
    cell->clicked(Point());

    ASSERT_FALSE(raised_item.has_value());
}

TEST(ItemsWindow, ItemSelectedRaisedWhenSyncItemEnabled)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    ItemsWindow window(device, shader_storage, font_factory, create_test_window(L"ItemsWindowTests"));

    std::optional<Item> raised_item;
    auto token = window.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window.set_items(items);

    auto list = window.root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(row, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 1);
}

TEST(ItemsWindow, ItemVisibilityRaised)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    ItemsWindow window(device, shader_storage, font_factory, create_test_window(L"ItemsWindowTests"));

    std::optional<std::tuple<Item, bool>> raised_item;
    auto token = window.on_item_visibility += [&raised_item](const auto& item, bool state) { raised_item = { item, state }; };

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window.set_items(items);

    auto list = window.root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Checkbox>(ui::Listbox::Row::Names::cell_name_format + "Hide");
    ASSERT_NE(row, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_FALSE(std::get<1>(raised_item.value()));
    ASSERT_EQ(std::get<0>(raised_item.value()).number(), 1);
}

TEST(ItemsWindow, ItemsListNotFilteredWhenRoomSetAndTrackRoomDisabled)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    ItemsWindow window(device, shader_storage, font_factory, create_test_window(L"ItemsWindowTests"));

    std::optional<Item> raised_item;
    auto token = window.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 55, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 78, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window.set_items(items);
    window.set_current_room(78);

    auto list = window.root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(row, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 0);
}

TEST(ItemsWindow, ItemsListFilteredWhenRoomSetAndTrackRoomEnabled)
{
    mocks::MockFontFactory font_factory;
    EXPECT_CALL(font_factory, create_font)
        .WillRepeatedly([](auto, auto, auto, auto) { return std::make_unique<mocks::MockFont>(); });

    Device device;
    ShaderStorage shader_storage;
    ItemsWindow window(device, shader_storage, font_factory, create_test_window(L"ItemsWindowTests"));

    std::optional<Item> raised_item;
    auto token = window.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 55, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 78, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window.set_items(items);
    window.set_current_room(78);

    auto track = window.root_control()->find<ui::Checkbox>(ItemsWindow::Names::track_room_checkbox);
    ASSERT_NE(track, nullptr);
    track->clicked(Point());

    auto list = window.root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(row, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 1);
}

TEST(ItemsWindow, ItemsListPopulatedOnSet)
{
    FAIL();
}

TEST(ItemsWindow, ItemsListUpdatedWhenFiltered)
{
    FAIL();
}

TEST(ItemsWindow, ItemsListUpdatedWhenNotFiltered)
{
    FAIL();
}

TEST(ItemsWindow, SelectionSurvivesFiltering)
{
    FAIL();
}

TEST(ItemsWindow, TriggersLoadedForItem)
{
    FAIL();
}

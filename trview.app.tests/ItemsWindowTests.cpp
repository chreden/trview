#include <trview.app/Windows/ItemsWindow.h>
#include <trview.tests.common/Window.h>
#include <trview.common/Size.h>
#include <trview.app/Elements/Types.h>
#include <trview.graphics/mocks/IDeviceWindow.h>
#include <trview.common/Mocks/Windows/IClipboard.h>
#include <trview.app/Mocks/Elements/ITrigger.h>
#include <trview.common/Mocks/Windows/IShell.h>

using namespace trview;
using namespace trview::tests;
using namespace trview::graphics;
using namespace trview::graphics::mocks;
using namespace trview::ui::render;
using namespace trview::ui::render::mocks;
using namespace trview::mocks;

namespace
{
    auto register_test_module()
    {
        struct test_module
        {
            std::shared_ptr<IClipboard> clipboard{ std::make_shared<MockClipboard>() };

            std::unique_ptr<ItemsWindow> build()
            {
                return std::make_unique<ItemsWindow>(clipboard);
            }
        };

        return test_module {};
    }
}
/*
TEST(ItemsWindow, AddToRouteEventRaised)
{
    auto window = register_test_module().build();

    std::optional<Item> raised_item;
    auto token = window->on_add_to_route += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);
    window->set_selected_item(items[1]);

    auto button = window->root_control()->find<ui::Button>(ItemsWindow::Names::add_to_route_button);
    ASSERT_NE(button, nullptr);

    button->clicked(Point());
    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 1);
}

TEST(ItemsWindow, ClearSelectedItemClearsSelection)
{
    auto window = register_test_module().build();

    std::optional<Item> raised_item;
    auto token = window->on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    auto trigger = std::make_shared<MockTrigger>();
    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, { trigger }, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);
    window->set_triggers({ trigger });

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 1);

    auto stats_list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::stats_listbox);
    ASSERT_NE(stats_list, nullptr);

    auto stats_items = stats_list->items();
    ASSERT_NE(stats_items.size(), 0);

    auto triggers_list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::triggers_listbox);
    ASSERT_NE(triggers_list, nullptr);

    auto triggers_items = triggers_list->items();
    ASSERT_NE(triggers_items.size(), 0);

    window->clear_selected_item();

    stats_items = stats_list->items();
    ASSERT_EQ(stats_items.size(), 0);

    triggers_items = triggers_list->items();
    ASSERT_EQ(triggers_items.size(), 0);
}

TEST(ItemsWindow, ItemSelectedNotRaisedWhenSyncItemDisabled)
{
    auto window = register_test_module().build();

    std::optional<Item> raised_item;
    auto token = window->on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);

    auto sync = window->root_control()->find<ui::Checkbox>(ItemsWindow::Names::sync_item_checkbox);
    ASSERT_NE(sync, nullptr);
    sync->clicked(Point());

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_FALSE(raised_item.has_value());
}

TEST(ItemsWindow, ItemSelectedRaisedWhenSyncItemEnabled)
{
    auto window = register_test_module().build();

    std::optional<Item> raised_item;
    auto token = window->on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 1);
}

TEST(ItemsWindow, ItemVisibilityRaised)
{
    auto window = register_test_module().build();

    std::optional<std::tuple<Item, bool>> raised_item;
    auto token = window->on_item_visibility += [&raised_item](const auto& item, bool state) { raised_item = { item, state }; };

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Checkbox>(ui::Listbox::Row::Names::cell_name_format + "Hide");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_FALSE(std::get<1>(raised_item.value()));
    ASSERT_EQ(std::get<0>(raised_item.value()).number(), 1);
}

TEST(ItemsWindow, ItemsListNotFilteredWhenRoomSetAndTrackRoomDisabled)
{
    auto window = register_test_module().build();

    std::optional<Item> raised_item;
    auto token = window->on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 55, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 78, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);
    window->set_current_room(78);

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 0);
}

TEST(ItemsWindow, ItemsListFilteredWhenRoomSetAndTrackRoomEnabled)
{
    auto window = register_test_module().build();

    std::optional<Item> raised_item;
    auto token = window->on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    std::vector<Item> items
    {
        Item(0, 55, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 78, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);
    window->set_current_room(78);

    auto track = window->root_control()->find<ui::Checkbox>(ItemsWindow::Names::track_room_checkbox);
    ASSERT_NE(track, nullptr);
    track->clicked(Point());

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().number(), 1);
}

TEST(ItemsWindow, ItemsListPopulatedOnSet)
{
    auto window = register_test_module().build();

    std::vector<Item> items
    {
        Item(0, 55, 0, L"Lara", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 78, 0, L"Winston", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    for (auto i = 0; i < items.size(); ++i)
    {
        const auto& item = items[i];
        auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + std::to_string(i));
        ASSERT_NE(row, nullptr);

        auto number_cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
        ASSERT_NE(number_cell, nullptr);
        ASSERT_EQ(number_cell->text(), std::to_wstring(item.number()));

        auto room_cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Room");
        ASSERT_NE(room_cell, nullptr);
        ASSERT_EQ(room_cell->text(), std::to_wstring(item.room()));

        auto type_cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Type");
        ASSERT_NE(type_cell, nullptr);
        ASSERT_EQ(type_cell->text(), item.type());
    }    
}

TEST(ItemsWindow, ItemsListUpdatedWhenFiltered)
{
    auto window = register_test_module().build();

    std::vector<Item> items
    {
        Item(0, 55, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 78, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);
    window->set_current_room(78);

    auto track = window->root_control()->find<ui::Checkbox>(ItemsWindow::Names::track_room_checkbox);
    ASSERT_NE(track, nullptr);
    track->clicked(Point());

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Checkbox>(ui::Listbox::Row::Names::cell_name_format + "Hide");
    ASSERT_NE(cell, nullptr);
    ASSERT_FALSE(cell->state());

    items[1].set_visible(false);
    window->update_items(items);

    cell = row->find<ui::Checkbox>(ui::Listbox::Row::Names::cell_name_format + "Hide");
    ASSERT_TRUE(cell->state());
}

TEST(ItemsWindow, ItemsListUpdatedWhenNotFiltered)
{
    auto window = register_test_module().build();

    std::vector<Item> items
    {
        Item(0, 55, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 78, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Checkbox>(ui::Listbox::Row::Names::cell_name_format + "Hide");
    ASSERT_NE(cell, nullptr);
    ASSERT_FALSE(cell->state());

    items[1].set_visible(false);
    window->update_items(items);

    cell = row->find<ui::Checkbox>(ui::Listbox::Row::Names::cell_name_format + "Hide");
    ASSERT_TRUE(cell->state());
}

TEST(ItemsWindow, SelectionSurvivesFiltering)
{
    auto window = register_test_module().build();

    std::vector<Item> items
    {
        Item(0, 55, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 78, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);
    window->set_current_room(78);

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    auto track = window->root_control()->find<ui::Checkbox>(ItemsWindow::Names::track_room_checkbox);
    ASSERT_NE(track, nullptr);
    track->clicked(Point());

    auto now_selected = list->selected_item();
    ASSERT_TRUE(now_selected.has_value());
    ASSERT_EQ(now_selected.value().value(L"#"), L"1");
}

TEST(ItemsWindow, TriggersLoadedForItem)
{
    auto window = register_test_module().build();

    auto trigger1 = std::make_shared<MockTrigger>()->with_number(0);
    auto trigger2 = std::make_shared<MockTrigger>()->with_number(1);
    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, { trigger1, trigger2 }, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);
    window->set_triggers({ trigger1, trigger2 });

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    auto triggers_list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::triggers_listbox);
    ASSERT_NE(triggers_list, nullptr);

    auto triggers_items = triggers_list->items();
    ASSERT_EQ(triggers_items.size(), 2);
    ASSERT_EQ(triggers_items[0].value(L"#"), L"0");
    ASSERT_EQ(triggers_items[1].value(L"#"), L"1");
}

TEST(ItemsWindow, TriggerSelectedEventRaised)
{
    auto window = register_test_module().build();

    std::optional<std::weak_ptr<ITrigger>> raised_trigger;
    auto token = window->on_trigger_selected += [&raised_trigger](const auto& trigger) { raised_trigger = trigger; };

    auto trigger = std::make_shared<MockTrigger>();
    std::vector<Item> items
    {
        Item(0, 0, 0, L"Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero),
        Item(1, 0, 0, L"Type", 0, 0, { trigger }, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);
    window->set_triggers({ trigger });

    auto list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::items_listbox);
    ASSERT_NE(list, nullptr);

    auto row = list->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(row, nullptr);

    auto cell = row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(row, nullptr);
    cell->clicked(Point());

    auto triggers_list = window->root_control()->find<ui::Listbox>(ItemsWindow::Names::triggers_listbox);
    ASSERT_NE(triggers_list, nullptr);

    auto triggers_row = triggers_list->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(triggers_row, nullptr);

    auto triggers_cell = triggers_row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(triggers_cell, nullptr);
    triggers_cell->clicked(Point());

    ASSERT_TRUE(raised_trigger.has_value());
    ASSERT_EQ(raised_trigger.value().lock(), trigger);
}

TEST(ItemsWindow, ClickStatShowsBubble)
{
    auto bubble = std::make_unique<MockBubble>();
    EXPECT_CALL(*bubble, show(testing::A<const Point&>())).Times(1);

    auto window = register_test_module().with_bubble_source([&](auto&&...) { return std::move(bubble); }).build();

    std::vector<Item> items
    {
        Item(0, 0, 0, L"Test Type", 0, 0, {}, DirectX::SimpleMath::Vector3::Zero)
    };
    window->set_items(items);
    window->set_selected_item(items[0]);

    auto stats = window->root_control()->find<Listbox>(ItemsWindow::Names::stats_listbox);
    ASSERT_NE(stats, nullptr);

    auto first_stat = stats->find<ui::Control>(Listbox::Names::row_name_format + "0");
    ASSERT_NE(first_stat, nullptr);

    auto value = first_stat->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Value");
    ASSERT_NE(value, nullptr);
    value->clicked(Point());
}
*/
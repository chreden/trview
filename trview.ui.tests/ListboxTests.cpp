#include "gtest/gtest.h"
#include <trview.ui/Listbox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Control.h>
#include <Windows.h>
#include <gmock\gmock-matchers.h>

using namespace trview;
using namespace trview::ui;

/// Tests that clicking on an item raises the item selected event.
TEST(Listbox, ClickItemRaisesEvent)
{
    Listbox listbox(Size(300, 200), Colour::Transparent);
    listbox.set_columns(
        {
            { Listbox::Column::IdentityMode::Key, Listbox::Column::Type::Number, L"#", 30 },
            { Listbox::Column::IdentityMode::None, Listbox::Column::Type::Boolean, L"Visible", 50 }
        });
    Listbox::Item item_1{ {{ L"#", L"1" }, { L"Visible", L"0" }} };
    Listbox::Item item_2{ {{ L"#", L"2" }, { L"Visible", L"1" }} };
    listbox.set_items({ item_1, item_2 });

    std::optional<Listbox::Item> raised_item;
    auto token = listbox.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    auto row_2 = listbox.find<Control>(Listbox::Names::row_name_format + "1");
    ASSERT_NE(row_2, nullptr);

    auto cell = row_2->find<Control>(Listbox::Row::Names::cell_name_format + "#");
    ASSERT_NE(cell, nullptr);
    cell->clicked(Point());

    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().value(L"#"), L"2");
}

/// Tests that clicking on a checkbox raises the item state changed event.
TEST(Listbox, ClickCheckboxRaisesEvent)
{
    Listbox listbox(Size(300, 200), Colour::Transparent);
    listbox.set_columns(
        {
            { Listbox::Column::IdentityMode::Key, Listbox::Column::Type::Number, L"#", 30 },
            { Listbox::Column::IdentityMode::None, Listbox::Column::Type::Boolean, L"Visible", 50 }
        });
    Listbox::Item item_1{ {{ L"#", L"1" }, { L"Visible", L"0" }} };
    Listbox::Item item_2{ {{ L"#", L"2" }, { L"Visible", L"1" }} };
    listbox.set_items({ item_1, item_2 });

    std::optional<std::tuple<Listbox::Item, std::wstring, bool>> raised_state;
    auto token = listbox.on_state_changed += [&raised_state](const auto& item, const auto& name, bool state) 
    {
        raised_state = { item, name, state };
    };

    auto row_2 = listbox.find<Control>(Listbox::Names::row_name_format + "1");
    ASSERT_NE(row_2, nullptr);

    auto check_2 = row_2->find<Checkbox>(Listbox::Row::Names::cell_name_format + "Visible");
    ASSERT_NE(check_2, nullptr);
    ASSERT_TRUE(check_2->state());

    check_2->clicked(Point());
    ASSERT_TRUE(raised_state.has_value());
    ASSERT_EQ(std::get<0>(raised_state.value()).value(L"#"), L"2");
    ASSERT_EQ(std::get<1>(raised_state.value()), L"Visible");
    ASSERT_EQ(std::get<2>(raised_state.value()), false);
}

/// Test that changing the selected item via keyboard input raises an event.
TEST(Listbox, KeyboardNavigationRaisesSelectedItemEvent)
{
    Listbox listbox(Size(300, 200), Colour::Transparent);
    listbox.set_columns(
        {
            { Listbox::Column::IdentityMode::Key, Listbox::Column::Type::Number, L"#", 30 },
            { Listbox::Column::IdentityMode::None, Listbox::Column::Type::Boolean, L"Visible", 50 }
        });
    Listbox::Item item_1{ {{ L"#", L"1" }, { L"Visible", L"0" }} };
    Listbox::Item item_2{ {{ L"#", L"2" }, { L"Visible", L"1" }} };
    listbox.set_items({ item_1, item_2 });

    std::optional<Listbox::Item> raised_item;
    auto token = listbox.on_item_selected += [&raised_item](const auto& item) { raised_item = item; };

    listbox.set_selected_item(item_1);
    listbox.key_down(VK_DOWN, false, false);
    ASSERT_TRUE(raised_item.has_value());
    ASSERT_EQ(raised_item.value().value(L"#"), L"2");
}

/// Tests that deleting an item raises an event.
TEST(Listbox, DeletingItemRaisesEvent)
{
    Listbox listbox(Size(300, 200), Colour::Transparent);

    bool raised = false;
    auto token = listbox.on_delete += [&raised]() { raised = true; };

    listbox.key_down(VK_DELETE, false, false);

    ASSERT_TRUE(raised);
}

/// Tests that the correct columns are created based on constructor input.
TEST(Listbox, ColumnsCreated)
{
    Listbox listbox(Size(300, 200), Colour::Transparent);
    listbox.set_columns(
        {
            { Listbox::Column::IdentityMode::Key, Listbox::Column::Type::Number, L"#", 30 },
            { Listbox::Column::IdentityMode::None, Listbox::Column::Type::String, L"Name", 50 },
            { Listbox::Column::IdentityMode::None, Listbox::Column::Type::Boolean, L"Value", 50 }
        });

    auto headers = listbox.find<Control>(Listbox::Names::header_container);
    ASSERT_NE(headers, nullptr);

    auto number_header = headers->find<Control>(Listbox::Names::header_name_format + "#");
    ASSERT_NE(number_header, nullptr);

    auto name_header = headers->find<Control>(Listbox::Names::header_name_format + "Name");
    ASSERT_NE(name_header, nullptr);

    auto value_header = headers->find<Control>(Listbox::Names::header_name_format + "Value");
    ASSERT_NE(value_header, nullptr);
}

/// Tests that only key identity columns are used for selection comparison.
TEST(Listbox, KeyIdentityUsedForSelection)
{
    Listbox listbox(Size(300, 200), Colour::Transparent);
    listbox.set_columns(
        {
            { Listbox::Column::IdentityMode::Key, Listbox::Column::Type::Number, L"#", 30 },
            { Listbox::Column::IdentityMode::None, Listbox::Column::Type::String, L"Name", 50 }
        });

    Listbox::Item item_1{ {{ L"#", L"100" }, { L"Name", L"First" }} };
    Listbox::Item item_2{ {{ L"#", L"200" }, { L"Name", L"Second" }} };
    Listbox::Item altered_item{ {{ L"#", L"100" }, { L"Name", L"First Renamed" }} };

    listbox.set_items({ item_1, item_2 });

    ASSERT_FALSE(listbox.selected_item().has_value());
    listbox.set_selected_item(item_1);
    auto selected_item = listbox.selected_item();
    ASSERT_TRUE(selected_item.has_value());
    ASSERT_EQ(selected_item.value().value(L"#"), L"100");

    listbox.set_selected_item(item_2);
    selected_item = listbox.selected_item();
    ASSERT_TRUE(selected_item.has_value());
    ASSERT_EQ(selected_item.value().value(L"#"), L"200");

    listbox.set_selected_item(altered_item);
    selected_item = listbox.selected_item();
    ASSERT_TRUE(selected_item.has_value());
    ASSERT_EQ(selected_item.value().value(L"#"), L"100");
}

/// Tests that ordering the listbox by a column functions.
TEST(Listbox, OrderingByColumn)
{
    Listbox listbox(Size(300, 200), Colour::Transparent);
    listbox.set_columns(
        {
            { Listbox::Column::IdentityMode::Key, Listbox::Column::Type::Number, L"#", 30 },
            { Listbox::Column::IdentityMode::None, Listbox::Column::Type::String, L"Name", 50 }
        });
    Listbox::Item item_1{ {{ L"#", L"1" }, { L"Name", L"Banana" }} };
    Listbox::Item item_2{ {{ L"#", L"2" }, { L"Name", L"Cheese" }} };
    Listbox::Item item_3{ {{ L"#", L"2" }, { L"Name", L"Apple" }} };
    listbox.set_items({ item_1, item_2, item_3 });

    auto get_names = [&listbox]()
    {
        std::vector<std::wstring> names;
        for (const auto& item : listbox.items()) 
        {
            names.push_back(item.value(L"Name"));
        }
        return names;
    };

    using ::testing::ElementsAre;
    ASSERT_THAT(get_names(), ElementsAre(L"Banana", L"Cheese", L"Apple"));

    auto headers = listbox.find<Control>(Listbox::Names::header_container);
    ASSERT_NE(headers, nullptr);

    auto name_header = headers->find<Control>(Listbox::Names::header_name_format + "Name");
    ASSERT_NE(headers, nullptr);
    
    name_header->clicked(Point());
    ASSERT_THAT(get_names(), ElementsAre(L"Apple", L"Banana", L"Cheese"));

    name_header->clicked(Point());
    ASSERT_THAT(get_names(), ElementsAre(L"Cheese", L"Banana", L"Apple"));
}

/// Checks that a checkbox is created when the column type is boolean.
TEST(Listbox, CheckboxCreatedForBoolean)
{
    Listbox listbox(Size(300, 200), Colour::Transparent);
    listbox.set_columns(
        {
            { Listbox::Column::IdentityMode::Key, Listbox::Column::Type::Number, L"#", 30 },
            { Listbox::Column::IdentityMode::None, Listbox::Column::Type::Boolean, L"Visible", 50 }
        });
    Listbox::Item item_1{ {{ L"#", L"1" }, { L"Visible", L"0" }} };
    Listbox::Item item_2{ {{ L"#", L"2" }, { L"Visible", L"1" }} };
    listbox.set_items({ item_1, item_2 });

    auto row_1 = listbox.find<Control>(Listbox::Names::row_name_format + "0");
    ASSERT_NE(row_1, nullptr);
    
    auto check_1 = row_1->find<Checkbox>(Listbox::Row::Names::cell_name_format + "Visible");
    ASSERT_NE(check_1, nullptr);
    ASSERT_FALSE(check_1->state());

    auto row_2 = listbox.find<Control>(Listbox::Names::row_name_format + "1");
    ASSERT_NE(row_2, nullptr);

    auto check_2 = row_2->find<Checkbox>(Listbox::Row::Names::cell_name_format + "Visible");
    ASSERT_NE(check_2, nullptr);
    ASSERT_TRUE(check_2->state());
}

TEST(Listbox, ClearSelectionClearsSelection)
{
    Listbox listbox(Size(300, 200), Colour::Transparent);
    listbox.set_columns(
        {
            { Listbox::Column::IdentityMode::Key, Listbox::Column::Type::Number, L"#", 30 },
            { Listbox::Column::IdentityMode::None, Listbox::Column::Type::String, L"Name", 50 }
        });

    Listbox::Item item_1{ {{ L"#", L"100" }, { L"Name", L"First" }} };
    listbox.set_items({ item_1 });

    ASSERT_FALSE(listbox.selected_item().has_value());

    listbox.set_selected_item(item_1);
    ASSERT_TRUE(listbox.selected_item().has_value());
    ASSERT_EQ(listbox.selected_item().value().value(L"#"), L"100");

    listbox.clear_selection();
    ASSERT_FALSE(listbox.selected_item().has_value());
}
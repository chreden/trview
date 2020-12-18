#include "gtest/gtest.h"
#include <trview.ui/Listbox.h>

using namespace trview;
using namespace trview::ui;

/// Tests that clicking on an item raises the item selected event.
TEST(Listbox, ClickItemRaisesEvent)
{
    FAIL();
}

/// Tests that clicking on a checkbox raises the item state changed event.
TEST(Listbox, ClickCheckboxRaisesEvent)
{
    FAIL();
}

/// Test that changing the selected item via keyboard input raises an event.
TEST(Listbox, KeyboardNavigationRaisesSelectedItemEvent)
{
    FAIL();
}

/// Tests that deleting an item raises an event.
TEST(Listbox, DeletingItemRaisesEvent)
{
    FAIL();
}

/// Tests that the correct columns are created based on constructor input.
TEST(Listbox, ColumnsCreated)
{
    FAIL();
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
    FAIL();
}

/// Checks that a checkbox is created when the column type is boolean.
TEST(Listbox, CheckboxCreatedForBoolean)
{
    FAIL();
}
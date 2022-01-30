#include <trview.app/UI/RoomNavigator.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <trview.app/Elements/RoomInfo.h>

using namespace trview;
using namespace trview::mocks;
/*
TEST(RoomNavigator, MaxRoomsLimitsCurrentRoom)
{
    ui::Window window(Size(), Colour::White);
    RoomNavigator navigator(window, JsonLoader(std::make_shared<MockShell>()));
    navigator.set_max_rooms(0);

    std::optional<uint32_t> raised;
    auto token = navigator.on_room_selected += [&](auto room)
    {
        raised = room;
    };

    auto current = window.find<NumericUpDown>(RoomNavigator::Names::current_room);
    ASSERT_NE(current, nullptr);

    auto up = current->find<Button>(NumericUpDown::Names::up);
    up->clicked(Point());

    ASSERT_FALSE(raised.has_value());
}

TEST(RoomNavigator, MaxRoomsUpdatesLabel)
{
    ui::Window window(Size(), Colour::White);
    RoomNavigator navigator(window, JsonLoader(std::make_shared<MockShell>()));
    navigator.set_max_rooms(0);

    auto max = window.find<Label>(RoomNavigator::Names::max_rooms);
    ASSERT_NE(max, nullptr);
    ASSERT_EQ(max->text(), L"0");

    navigator.set_max_rooms(100);
    ASSERT_EQ(max->text(), L"99");
}

TEST(RoomNavigator, CoordinatesDisplayed)
{
    ui::Window window(Size(), Colour::White);
    RoomNavigator navigator(window, JsonLoader(std::make_shared<MockShell>()));

    RoomInfo info;
    info.x = 1024;
    info.z = 2048;
    navigator.set_room_info(info);

    auto stats = window.find<Listbox>(RoomNavigator::Names::stats);
    ASSERT_NE(stats, nullptr);

    auto x_row = stats->find<ui::Control>(ui::Listbox::Names::row_name_format + "0");
    ASSERT_NE(x_row, nullptr);

    auto x_cell = x_row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Value");
    ASSERT_NE(x_cell, nullptr);

    auto z_row = stats->find<ui::Control>(ui::Listbox::Names::row_name_format + "1");
    ASSERT_NE(z_row, nullptr);

    auto z_cell = z_row->find<ui::Button>(ui::Listbox::Row::Names::cell_name_format + "Value");
    ASSERT_NE(z_cell, nullptr);

    ASSERT_EQ(x_cell->text(), L"1");
    ASSERT_EQ(z_cell->text(), L"2");
}

TEST(RoomNavigator, RoomSelectedEventRaised)
{
    ui::Window window(Size(), Colour::White);
    RoomNavigator navigator(window, JsonLoader(std::make_shared<MockShell>()));

    std::optional<uint32_t> raised;
    auto token = navigator.on_room_selected += [&](auto room)
    {
        raised = room;
    };

    navigator.set_max_rooms(10);

    auto current = window.find<NumericUpDown>(RoomNavigator::Names::current_room);
    ASSERT_NE(current, nullptr);

    auto up = current->find<Button>(NumericUpDown::Names::up);
    up->clicked(Point());

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 1);
}

TEST(RoomNavigator, SetCurrentRoomUpdatesUpDown)
{
    ui::Window window(Size(), Colour::White);
    RoomNavigator navigator(window, JsonLoader(std::make_shared<MockShell>()));
    navigator.set_max_rooms(10);
    auto current = window.find<NumericUpDown>(RoomNavigator::Names::current_room);
    ASSERT_NE(current, nullptr);
    ASSERT_EQ(current->value(), 0);

    navigator.set_selected_room(1);
    ASSERT_EQ(current->value(), 1);
}
*/
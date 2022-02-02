#include <trview.app/UI/RoomNavigator.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;

TEST(RoomNavigator, MinRoomsLimitsCurrentRoom)
{
    RoomNavigator navigator;
    navigator.set_max_rooms(10);

    std::optional<uint32_t> raised;
    auto token = navigator.on_room_selected += [&](auto room)
    {
        raised = room;
    };

    tests::TestImgui imgui([&]() { navigator.render(); });
    imgui.click_element("Room Navigator", { "of 0##roomnumber", "-" });

    ASSERT_FALSE(raised.has_value());
}

TEST(RoomNavigator, MaxRoomsLimitsCurrentRoom)
{
    RoomNavigator navigator;
    navigator.set_max_rooms(0);

    std::optional<uint32_t> raised;
    auto token = navigator.on_room_selected += [&](auto room)
    {
        raised = room;
    };

    tests::TestImgui imgui([&]() { navigator.render(); });
    imgui.click_element("Room Navigator", { "of 0##roomnumber", "+" });

    ASSERT_FALSE(raised.has_value());
}

TEST(RoomNavigator, MaxRoomsUpdatesLabel)
{
    RoomNavigator navigator;
    navigator.set_max_rooms(0);

    tests::TestImgui imgui([&]() { navigator.render(); });
    ASSERT_TRUE(imgui.element_present("Room Navigator", { "of 0##roomnumber" }));
    imgui.reset();

    navigator.set_max_rooms(100);
    imgui.render();
    ASSERT_TRUE(imgui.element_present("Room Navigator", { "of 99##roomnumber" }));
}

TEST(RoomNavigator, RoomSelectedEventRaisedOnMinus)
{
    RoomNavigator navigator;

    std::optional<uint32_t> raised;
    auto token = navigator.on_room_selected += [&](auto room)
    {
        raised = room;
    };

    navigator.set_selected_room(5);
    navigator.set_max_rooms(10);

    tests::TestImgui imgui([&]() { navigator.render(); });
    imgui.click_element("Room Navigator", { "of 9##roomnumber", "-" });

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 4);
}

TEST(RoomNavigator, RoomSelectedEventRaisedOnPlus)
{
    RoomNavigator navigator;

    std::optional<uint32_t> raised;
    auto token = navigator.on_room_selected += [&](auto room)
    {
        raised = room;
    };

    navigator.set_max_rooms(10);

    tests::TestImgui imgui([&]() { navigator.render(); });
    imgui.click_element("Room Navigator", { "of 9##roomnumber", "+" });

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 1);
}

TEST(RoomNavigator, SetCurrentRoomUpdatesUpDown)
{
    RoomNavigator navigator;

    navigator.set_max_rooms(10);
    tests::TestImgui imgui([&]() { navigator.render(); });
    ASSERT_EQ(imgui.item_text("Room Navigator", { "of 9##roomnumber", "" }), "0");

    navigator.set_selected_room(1);
    imgui.render();
    ASSERT_EQ(imgui.item_text("Room Navigator", { "of 9##roomnumber", "" }), "1");
}

TEST(RoomNavigator, EnterRoomNumber)
{
    RoomNavigator navigator;

    std::optional<uint32_t> raised;
    auto token = navigator.on_room_selected += [&](auto room)
    {
        raised = room;
    };

    navigator.set_max_rooms(10);
    tests::TestImgui imgui([&]() { navigator.render(); });
    imgui.enter_text("Room Navigator", { "of 9##roomnumber", "" }, "5");
 
    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 5);
}
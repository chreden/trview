#include <trview.app/UI/GoTo.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;

TEST(GoTo, Name)
{
    GoTo window;
    window.toggle_visible();

    ASSERT_EQ(window.name(), "");
    window.set_name("Item");
    ASSERT_EQ(window.name(), "Item");

    TestImgui imgui([&]() { window.render(); });
    ASSERT_NE(imgui.find_window(imgui.popup_id("Go To Item").name()), nullptr);
}

TEST(GoTo, OnSelectedNotRaisedWhenMinusPressedAtZero)
{
    GoTo window;
    window.toggle_visible();
    window.set_name("Item");

    std::optional<uint32_t> raised;
    auto token = window.on_selected += [&](auto value)
    {
        raised = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(
        imgui.popup_id("Go To Item").push("##gotoentry").id("-"),
        false,
        imgui.popup_id("Go To Item").id("##gotoentry"));

    ASSERT_FALSE(raised.has_value());
}

TEST(GoTo, OnSelectedRaisedNumber)
{
    GoTo window;
    window.toggle_visible();
    window.set_name("Item");
    window.set_items({ { .number = 10, .name = "Room Ten" } });
    std::optional<uint32_t> raised;
    auto token = window.on_selected += [&](auto value)
    {
        raised = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.popup_id("Go To Item").id("##gotoentry"));
    imgui.enter_text("10");
    imgui.press_key(ImGuiKey_DownArrow);
    imgui.press_key(ImGuiKey_Enter);
    imgui.reset();
    imgui.render();

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 10u);
    ASSERT_FALSE(window.visible());
}

TEST(GoTo, OnSelectedRaisedText)
{
    GoTo window;
    window.toggle_visible();
    window.set_name("Item");
    window.set_items({ {.number = 10, .name = "Room Ten" } });
    std::optional<uint32_t> raised;
    auto token = window.on_selected += [&](auto value)
    {
        raised = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.popup_id("Go To Item").id("##gotoentry"));
    imgui.enter_text("Ten");
    imgui.press_key(ImGuiKey_DownArrow);
    imgui.press_key(ImGuiKey_Enter);
    imgui.reset();
    imgui.render();

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 10u);
    ASSERT_FALSE(window.visible());
}

TEST(GoTo, OnSelectedNotRaisedWhenCancelled)
{
    GoTo window;
    window.toggle_visible();
    window.set_name("Item");

    std::optional<uint32_t> raised;
    auto token = window.on_selected += [&](auto value)
    {
        raised = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.popup_id("Go To Item").id("##gotoentry"));
    imgui.press_key(ImGuiKey_Escape);

    ASSERT_FALSE(raised.has_value());
    ASSERT_FALSE(window.visible());
}

#include <trview.app/UI/GoTo.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;

TEST(GoTo, OnSelectedRaisedNumber)
{
    GoTo window;
    window.toggle_visible();
    window.set_items({ { .number = 10, .name = "Room Ten" } });
    std::optional<GoTo::GoToItem> raised;
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
    ASSERT_EQ(raised.value().number, 10u);
    ASSERT_FALSE(window.visible());
}

TEST(GoTo, OnSelectedRaisedText)
{
    GoTo window;
    window.toggle_visible();
    window.set_items({ {.number = 10, .name = "Room Ten" } });
    std::optional<GoTo::GoToItem> raised;
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
    ASSERT_EQ(raised.value().number, 10u);
    ASSERT_FALSE(window.visible());
}

TEST(GoTo, OnSelectedNotRaisedWhenCancelled)
{
    GoTo window;
    window.toggle_visible();

    std::optional<GoTo::GoToItem> raised;
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

#include <trview.app/UI/GoTo.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;

TEST(GoTo, Name)
{
    GoTo window;
    window.toggle_visible(0);

    ASSERT_EQ(window.name(), "");
    window.set_name("Item");
    ASSERT_EQ(window.name(), "Item");

    TestImgui imgui([&]() { window.render(); });
    ASSERT_NE(imgui.find_window(imgui.popup_id("Go To Item").name()), nullptr);
}

TEST(GoTo, OnSelectedWithPlusRaised)
{
    GoTo window;
    window.toggle_visible(0);
    window.set_name("Item");

    std::optional<uint32_t> raised;
    auto token = window.on_selected += [&](auto value)
    {
        raised = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(
        imgui.popup_id("Go To Item").push("##gotoentry").id("+"), 
        false, 
        imgui.popup_id("Go To Item").id("##gotoentry"));

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 1u);
}

TEST(GoTo, OnSelectedWithMinusRaised)
{
    GoTo window;
    window.toggle_visible(0);
    window.set_name("Item");

    std::vector<uint32_t> raised;
    auto token = window.on_selected += [&](auto value)
    {
        raised.push_back(value);
    };

    TestImgui imgui([&]() { window.render(); });
    const auto goto_entry = imgui.popup_id("Go To Item").id("##gotoentry");
    const auto plus = imgui.popup_id("Go To Item").push("##gotoentry").id("+");
    const auto minus = imgui.popup_id("Go To Item").push("##gotoentry").id("-");

    imgui.click_element(plus, false, goto_entry);
    imgui.click_element(plus, false, goto_entry);
    imgui.click_element(minus, false, goto_entry);

    const std::vector<uint32_t> expected{ 1, 2, 1 };
    ASSERT_EQ(raised, expected);
}

TEST(GoTo, OnSelectedNotRaisedWhenMinusPressedAtZero)
{
    GoTo window;
    window.toggle_visible(0);
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

TEST(GoTo, OnSelectedRaised)
{
    GoTo window;
    window.toggle_visible(0);
    window.set_name("Item");

    std::optional<uint32_t> raised;
    auto token = window.on_selected += [&](auto value)
    {
        raised = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.popup_id("Go To Item").id("##gotoentry"));
    imgui.enter_text("10");
    imgui.press_key(ImGuiKey_Enter);

    imgui.reset();
    imgui.render();

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 10u);
    ASSERT_FALSE(window.visible());
}

TEST(GoTo, OnZeroSelectedRaised)
{
    GoTo window;
    window.toggle_visible(0);
    window.set_name("Item");

    std::optional<uint32_t> raised;
    auto token = window.on_selected += [&](auto value)
    {
        raised = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.popup_id("Go To Item").id("##gotoentry"));
    imgui.enter_text("0");
    imgui.press_key(ImGuiKey_Enter);

    imgui.reset();
    imgui.render();

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 0u);
    ASSERT_FALSE(window.visible());
}

TEST(GoTo, OnSelectedNotRaisedWhenCancelled)
{
    GoTo window;
    window.toggle_visible(0);
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

TEST(GoTo, OnSelectedNotRaisedOnNegative)
{
    GoTo window;
    window.toggle_visible(0);
    window.set_name("Item");

    std::optional<uint32_t> raised;
    auto token = window.on_selected += [&](auto value)
    {
        raised = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.popup_id("Go To Item").id("##gotoentry"));
    imgui.enter_text("-10");
    imgui.press_key(ImGuiKey_Enter);

    imgui.reset();
    imgui.render();

    ASSERT_FALSE(raised.has_value());
    ASSERT_FALSE(window.visible());
}
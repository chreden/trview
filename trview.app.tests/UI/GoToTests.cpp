#include <trview.app/UI/GoTo.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;

TEST(GoTo, Name)
{
    GoTo window;
    window.toggle_visible();

    ASSERT_EQ(window.name(), L"");
    window.set_name(L"Item");
    ASSERT_EQ(window.name(), L"Item");

    TestImgui imgui([&]() { window.render(); });
    ASSERT_NE(imgui.find_window(imgui.popup_name("Go To Item")), nullptr);
}
/*
TEST(GoTo, OnSelectedRaised)
{
    GoTo window;
    window.toggle_visible();
    window.set_name(L"Item");

    std::optional<uint32_t> raised;
    auto token = window.on_selected += [&](auto value)
    {
        raised = value;
    };

    TestImgui imgui([&]() { window.render(); });
    imgui.click_element(imgui.popup_name("Go To Item"), { "##gotoentry", "+" });

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 1u);
    ASSERT_FALSE(window.visible());
}

TEST(GoTo, OnSelectedNotRaisedWhenCancelled)
{
    ui::Window parent(Size(), Colour::Transparent);
    GoTo window(parent, JsonLoader(std::make_shared<MockShell>()));
    window.toggle_visible();

    std::optional<uint32_t> raised;
    auto token = window.on_selected += [&](auto value)
    {
        raised = value;
    };

    auto text_area = parent.find<TextArea>(GoTo::Names::text_area);
    ASSERT_NE(text_area, nullptr);
    text_area->gained_focus();
    text_area->set_text(L"100");
    text_area->key_char(VK_ESCAPE);

    ASSERT_FALSE(raised.has_value());
    ASSERT_FALSE(window.visible());
}

TEST(GoTo, Visible)
{
    ui::Window parent(Size(), Colour::Transparent);
    GoTo window(parent, JsonLoader(std::make_shared<MockShell>()));
    window.toggle_visible();

    auto text_area = parent.find<TextArea>(GoTo::Names::text_area);
    ASSERT_NE(text_area, nullptr);

    ASSERT_TRUE(window.visible());
    ASSERT_TRUE(text_area->visible(true));

    window.toggle_visible();
    ASSERT_FALSE(window.visible());
    ASSERT_FALSE(text_area->visible(true));
}
*/
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <trview.ui/TextArea.h>
#include <trview.ui/json.h>
#include <optional>

using namespace trview;
using namespace trview::ui;

TEST(TextArea, LoadFromJson)
{
    const std::string json = "{\"type\":\"textarea\",\"show_scrollbar\":false,\"text_alignment\":\"centre\",\"read_only\":true,\"line_mode\":\"single\",\"text\":\"Test\",\"size\":{\"width\":100,\"height\":100}}";
    auto control = ui::load_from_json(json);
    ASSERT_NE(control, nullptr);
    auto textarea = dynamic_cast<TextArea*>(control.get());
    ASSERT_NE(textarea, nullptr);
    ASSERT_EQ(textarea->scrollbar_visible(), false);
    ASSERT_EQ(textarea->text_alignment(), graphics::TextAlignment::Centre);
    ASSERT_EQ(textarea->read_only(), true);
    ASSERT_EQ(textarea->line_mode(), TextArea::Mode::SingleLine);
    ASSERT_EQ(textarea->text(), L"Test");
}

TEST(TextArea, OnTextChangedRaised)
{
    FAIL();
}

TEST(TextArea, OnEnterRaised)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White);
    text_area.set_mode(TextArea::Mode::SingleLine);
    text_area.gained_focus();

    std::optional<std::wstring> raised;
    auto token = text_area.on_enter += [&](const std::wstring& value)
    {
        raised = value;
    };

    text_area.set_text(L"Some text");
    text_area.key_char(VK_RETURN);

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), L"Some text");
}

TEST(TextArea, OnEnterNotRaisedInMultiLine)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White);
    text_area.gained_focus();

    std::optional<std::wstring> raised;
    auto token = text_area.on_enter += [&](const std::wstring& value)
    {
        raised = value;
    };

    text_area.set_text(L"Some text");
    text_area.key_char(VK_RETURN);

    ASSERT_FALSE(raised.has_value());
    ASSERT_EQ(text_area.text(), L"Some text\n");
}

TEST(TextArea, OnEscapeRaised)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White);
    text_area.set_mode(TextArea::Mode::SingleLine);
    text_area.gained_focus();

    bool raised = false;
    auto token = text_area.on_escape += [&]()
    {
        raised = true;
    };

    text_area.key_char(VK_ESCAPE);

    ASSERT_TRUE(raised);
}

TEST(TextArea, OnTabRaised)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White);
    text_area.set_mode(TextArea::Mode::SingleLine);
    text_area.gained_focus();

    std::optional<std::wstring> raised;
    auto token = text_area.on_tab += [&](const std::wstring& value)
    {
        raised = value;
    };

    text_area.set_text(L"Some text");
    text_area.key_char(VK_TAB);

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), L"Some text");
}

TEST(TextArea, OnTabNotRaisedInMultiLine)
{
    FAIL();
}

TEST(TextArea, OnClickRaised)
{
    FAIL();
}

TEST(TextArea, Text)
{
    FAIL();
}

TEST(TextArea, Mode) 
{
    FAIL();
}

TEST(TextArea, ReadOnly)
{
    FAIL();
}

TEST(TextArea, CopySelection)
{
    FAIL();
}

TEST(TextArea, DeleteSelection)
{
    FAIL();
}

TEST(TextArea, Paste)
{
    FAIL();
}

TEST(TextArea, LinkClickable)
{
    FAIL();
}

TEST(TextArea, TextAlignment)
{
    FAIL();
}

TEST(TextArea, Typing)
{
    FAIL();
}
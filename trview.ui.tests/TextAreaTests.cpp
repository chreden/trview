#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <trview.ui/TextArea.h>
#include <trview.ui/JsonLoader.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <optional>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

TEST(TextArea, LoadFromJson)
{
    const std::string json = "{\"type\":\"textarea\",\"show_scrollbar\":false,\"text_alignment\":\"centre\",\"read_only\":true,\"line_mode\":\"single\",\"text\":\"Test\",\"size\":{\"width\":100,\"height\":100}}";
    auto control = JsonLoader(std::make_shared<MockShell>()).load_from_json(json);
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
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    text_area.gained_focus();

    std::vector<std::wstring> raised;
    auto token = text_area.on_text_changed += [&](const auto& value)
    {
        raised.push_back(value);
    };

    for (const auto& c : std::wstring(L"Test"))
    {
        text_area.key_char(c);
    }

    ASSERT_EQ(raised.size(), 4);
    const std::vector<std::wstring> expected{ L"T", L"Te", L"Tes", L"Test" };
    ASSERT_EQ(raised, expected);
}

TEST(TextArea, OnEnterRaised)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
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
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
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
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
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
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
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
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());

    bool raised = false;
    auto token = text_area.on_click += [&]()
    {
        raised = true;
    };

    text_area.key_char(VK_TAB);
    ASSERT_FALSE(raised);
}

TEST(TextArea, OnClickRaised)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    text_area.set_text(L"Test");

    bool raised = false;
    auto token = text_area.on_click += [&]()
    {
        raised = true;
    };

    text_area.clicked(Point());
    ASSERT_TRUE(raised);
}

TEST(TextArea, Text)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    ASSERT_EQ(text_area.text(), L"");
    text_area.set_text(L"Test message");
    ASSERT_EQ(text_area.text(), L"Test message");
}

TEST(TextArea, Mode)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    ASSERT_EQ(text_area.line_mode(), TextArea::Mode::MultiLine);
    text_area.set_mode(TextArea::Mode::SingleLine);
    ASSERT_EQ(text_area.line_mode(), TextArea::Mode::SingleLine);
}

TEST(TextArea, ReadOnly)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    text_area.gained_focus();
    text_area.set_text(L"Test message");
    text_area.set_read_only(true);

    text_area.key_char(L'X');
    ASSERT_EQ(text_area.text(), L"Test message");
}

TEST(TextArea, CopySelection)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    text_area.gained_focus();
    text_area.set_text(L"Test message");
    text_area.highlight_all();

    std::wstring text;
    bool copied = text_area.copy(text);

    ASSERT_TRUE(copied);
    ASSERT_EQ(text, L"Test message");
}

TEST(TextArea, CopyNoSelection)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    text_area.gained_focus();
    text_area.set_text(L"Test message");

    std::wstring text;
    bool copied = text_area.copy(text);

    ASSERT_FALSE(copied);
}

TEST(TextArea, DeleteSelection)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    text_area.gained_focus();
    text_area.set_text(L"Test message");

    text_area.key_down(VK_HOME, false, false);
    text_area.key_down(VK_RIGHT, true, true);
    text_area.key_down(VK_DELETE, false, false);
    ASSERT_EQ(text_area.text(), L" message");
}

TEST(TextArea, Paste)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    text_area.gained_focus();
    text_area.paste(L"Test message");
    ASSERT_EQ(text_area.text(), L"Test message");
}

TEST(TextArea, PasteAtStart)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    text_area.gained_focus();
    text_area.set_text(L"message");
    text_area.key_down(VK_HOME, false, false);
    text_area.paste(L"Test ");
    ASSERT_EQ(text_area.text(), L"Test message");
}

TEST(TextArea, LinkClickable)
{
    auto shell = std::make_shared<MockShell>();
    EXPECT_CALL(*shell, open(std::wstring(L"http://example.com"))).Times(1);

    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, shell);
    text_area.gained_focus();
    text_area.set_text(L"http://example.com");
    text_area.clicked(Point(5, 5));
}

TEST(TextArea, LinkWithCtrlEnter)
{
    auto shell = std::make_shared<MockShell>();
    EXPECT_CALL(*shell, open(std::wstring(L"http://example.com"))).Times(1);

    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, shell);
    text_area.gained_focus();
    text_area.set_text(L"http://example.com");
    text_area.key_down(VK_HOME, false, false);
    text_area.key_char(0xA);
}

TEST(TextArea, TextAlignment)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    ASSERT_EQ(text_area.text_alignment(), graphics::TextAlignment::Left);
    text_area.set_text_alignment(graphics::TextAlignment::Right);
    ASSERT_EQ(text_area.text_alignment(), graphics::TextAlignment::Right);
}

TEST(TextArea, Typing)
{
    TextArea text_area(Size(100, 20), Colour::Black, Colour::White, std::make_shared<MockShell>());
    text_area.gained_focus();

    for (const auto& c : std::wstring(L"Test message"))
    {
        text_area.key_char(c);
    }

    ASSERT_EQ(text_area.text(), L"Test message");
}
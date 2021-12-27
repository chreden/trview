#include <trview.ui/Button.h>
#include <trview.ui/JsonLoader.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

/// Tests that the click event is raised when the button is clicked on.
TEST(Button, ClickEventRaised)
{
    Button button(Point(), Size(20, 20));
    bool raised = false;
    auto token = button.on_click += [&raised]() { raised = true; };
    button.clicked(Point());
    ASSERT_TRUE(raised);
}

TEST(Button, LoadFromJson)
{
    const std::string json = "{\"type\":\"button\",\"text\":\"Test\",\"text_colour\":\"red\",\"text_background_colour\":\"blue\"}";
    auto control = JsonLoader(std::make_shared<MockShell>()).load_from_json(json);
    ASSERT_NE(control, nullptr);
    auto button = dynamic_cast<Button*>(control.get());
    ASSERT_NE(button, nullptr);
    ASSERT_EQ(button->text(), L"Test");
    ASSERT_EQ(button->text_colour(), Colour::Red);
    ASSERT_EQ(button->text_background_colour(), Colour::Blue);
}

TEST(Button, BorderThickness)
{
    Button button(Size(10, 10), L"Test");
    button.set_border_thickness(5.0f);
    ASSERT_EQ(button.border_thickness(), 5.0f);
}

TEST(Button, SetText)
{
    Button button(Size(10, 10), L"Test");

    auto text = button.find<Label>(Button::Names::text);
    ASSERT_NE(text, nullptr);
    ASSERT_EQ(text->text(), L"Test");

    button.set_text(L"Updated");
    ASSERT_EQ(text->text(), L"Updated");
}

TEST(Button, UpdateButtonToHaveText)
{
    Button button(Size(10, 10));

    auto text = button.find<Label>(Button::Names::text);
    ASSERT_EQ(text, nullptr);

    button.set_text(L"Test");

    text = button.find<Label>(Button::Names::text);
    ASSERT_NE(text, nullptr);
    ASSERT_EQ(text->text(), L"Test");
}

TEST(Button, SetTextBackgroundColour)
{
    Button button(Size(10, 10), L"Test");
    button.set_text_background_colour(Colour::Red);

    auto text = button.find<Label>(Button::Names::text);
    ASSERT_NE(text, nullptr);
    ASSERT_EQ(text->background_colour(), Colour::Red);
}

TEST(Button, SetTextColour)
{
    Button button(Size(10, 10), L"Test");
    button.set_text_colour(Colour::Red);

    auto text = button.find<Label>(Button::Names::text);
    ASSERT_NE(text, nullptr);
    ASSERT_EQ(text->text_colour(), Colour::Red);
}

TEST(Button, HighlightOnMouseOver)
{
    Button button(Size(10, 10), L"Test");
    button.set_text_background_colour(Colour::Black);
    ASSERT_EQ(button.text_background_colour(), Colour::Black);

    button.mouse_enter();
    ASSERT_NE(button.text_background_colour(), Colour::Black);

    button.mouse_leave();
    ASSERT_EQ(button.text_background_colour(), Colour::Black);
}

TEST(Button, SetFont)
{
    Button button(Size(), L"Text");

    auto label = button.find<Label>(Button::Names::text);
    ASSERT_NE(label, nullptr);

    const std::string original = label->font();

    button.set_font("Test font");
    ASSERT_EQ(label->font(), "Test font");
}

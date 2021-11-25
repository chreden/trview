#include "gtest/gtest.h"
#include <trview.ui/Button.h>
#include <trview.ui/json.h>

using namespace trview;
using namespace trview::ui;

/// Tests that the click event is raised when the button is clicked on.
TEST(Button, ClickEventRaised)
{
    Button button(Point(), Size(20, 20), graphics::Texture(), graphics::Texture());
    bool raised = false;
    auto token = button.on_click += [&raised]() { raised = true; };
    button.clicked(Point());
    ASSERT_TRUE(raised);
}

TEST(Button, LoadFromJson)
{
    const std::string json = "{\"type\":\"button\",\"text\":\"Test\",\"text_colour\":\"red\",\"text_background_colour\":\"blue\"}";
    auto control = ui::load_from_json(json);
    ASSERT_NE(control, nullptr);
    auto button = dynamic_cast<Button*>(control.get());
    ASSERT_NE(button, nullptr);
    ASSERT_EQ(button->text(), L"Test");
    ASSERT_EQ(button->text_colour(), Colour::Red);
    ASSERT_EQ(button->text_background_colour(), Colour::Blue);
}

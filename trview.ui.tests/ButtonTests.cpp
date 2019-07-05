#include "gtest/gtest.h"
#include <trview.ui/Button.h>

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

#include "gtest/gtest.h"

#include <trview.ui/Window.h>
#include <trview.ui/Layouts/GridLayout.h>

using namespace trview;
using namespace trview::ui;

/// Tests that controls are placed correctly when added to the grid.
TEST(GridLayout, ControlsPositionedCorrectly)
{
    ui::Window window(Size(100, 200), Colour::White);

    window.set_layout(std::make_unique<GridLayout>(2, 2));

    auto x1y1 = window.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::Transparent));
    auto x2y1 = window.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::Transparent));
    auto x1y2 = window.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::Transparent));
    auto x2y2 = window.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::Transparent));

    ASSERT_EQ(x1y1->absolute_position(), Point(0, 0));
    ASSERT_EQ(x1y1->position(), Point(0, 0));
    ASSERT_EQ(x2y1->absolute_position(), Point(50, 0));
    ASSERT_EQ(x2y1->position(), Point(50, 0));
    ASSERT_EQ(x1y2->absolute_position(), Point(0, 100));
    ASSERT_EQ(x1y2->position(), Point(0, 100));
    ASSERT_EQ(x2y2->absolute_position(), Point(50, 100));
    ASSERT_EQ(x2y2->position(), Point(50, 100));
}

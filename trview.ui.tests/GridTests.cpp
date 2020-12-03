#include "gtest/gtest.h"
#include <trview.ui/Grid.h>

using namespace trview;
using namespace trview::ui;

/// Tests that controls are placed correctly when added to the grid.
TEST(Grid, ControlsPositionedCorrectly)
{
    Grid grid(Size(100, 200), Colour::Transparent, 2, 2);

    auto x1y1 = grid.add_child(std::make_unique<Window>(Size(10, 10), Colour::Transparent));
    auto x2y1 = grid.add_child(std::make_unique<Window>(Size(10, 10), Colour::Transparent));
    auto x1y2 = grid.add_child(std::make_unique<Window>(Size(10, 10), Colour::Transparent));
    auto x2y2 = grid.add_child(std::make_unique<Window>(Size(10, 10), Colour::Transparent));

    ASSERT_EQ(x1y1->absolute_position(), Point(0, 0));
    ASSERT_EQ(x1y1->position(), Point());
    ASSERT_EQ(x2y1->absolute_position(), Point(50, 0));
    ASSERT_EQ(x2y1->position(), Point());
    ASSERT_EQ(x1y2->absolute_position(), Point(0, 100));
    ASSERT_EQ(x1y2->position(), Point());
    ASSERT_EQ(x2y2->absolute_position(), Point(50, 100));
    ASSERT_EQ(x2y2->position(), Point());
}
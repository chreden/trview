#include <gmock/gmock.h>
#include "gtest/gtest.h"

#include <trview.ui/Window.h>
#include <trview.ui/Layouts/GridLayout.h>
#include <trview.ui/JsonLoader.h>
#include <trview.common/Mocks/Windows/IShell.h>

using namespace trview;
using namespace trview::mocks;
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

TEST(GridLayout, LoadFromJson)
{
    const std::string json = "{\"type\":\"window\", \"layout\":{\"type\":\"grid\",\"rows\":4,\"columns\":2}}";

    auto loaded = JsonLoader(std::make_shared<MockShell>()).load_from_json(json);
    ASSERT_NE(loaded, nullptr);

    const auto layout = loaded->layout();
    ASSERT_NE(layout, nullptr);
    auto grid = dynamic_cast<const GridLayout*>(layout);
    ASSERT_NE(grid, nullptr);
    ASSERT_EQ(grid->rows(), 4);
    ASSERT_EQ(grid->columns(), 2);
}

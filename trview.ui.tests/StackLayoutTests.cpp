#include "gtest/gtest.h"

#include <trview.ui/Window.h>
#include <trview.ui/Layouts/StackLayout.h>

using namespace trview;
using namespace trview::ui;

// Tests that padding is correctly applied in between elements.
TEST(StackLayout, PaddingAppliedVertically)
{
    ui::Window control(Size(), Colour::White);
    auto layout = std::make_unique<StackLayout>(10, StackLayout::Direction::Vertical);
    control.set_layout(std::move(layout));

    auto first_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));
    auto second_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));

    ASSERT_EQ(first_element->position(), Point(0, 0));
    ASSERT_EQ(second_element->position(), Point(0, 20));
}

// Tests that padding is correctly applied in between elements.
TEST(StackLayout, PaddingAppliedHorizontally)
{
    ui::Window control(Size(), Colour::White);
    auto layout = std::make_unique<StackLayout>(10, StackLayout::Direction::Horizontal);
    control.set_layout(std::move(layout));

    auto first_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));
    auto second_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));

    ASSERT_EQ(first_element->position(), Point(0, 0));
    ASSERT_EQ(second_element->position(), Point(20, 0));
}

// Tests that the elements are stacked in the correct direction.
TEST(StackLayout, DirectionHorizontal)
{
    ui::Window control(Size(), Colour::White);
    auto layout = std::make_unique<StackLayout>(0, StackLayout::Direction::Horizontal);
    control.set_layout(std::move(layout));

    auto first_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));
    auto second_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));

    ASSERT_EQ(first_element->position(), Point(0, 0));
    ASSERT_EQ(second_element->position(), Point(10, 0));
}

// Tests that the elements are stacked in the correct direction.
TEST(StackLayout, DirectionVertical)
{
    ui::Window control(Size(), Colour::White);
    auto layout = std::make_unique<StackLayout>(0, StackLayout::Direction::Vertical);
    control.set_layout(std::move(layout));

    auto first_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));
    auto second_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));

    ASSERT_EQ(first_element->position(), Point(0, 0));
    ASSERT_EQ(second_element->position(), Point(0, 10));
}

// Tests that when auto size is on the host control is resized.
TEST(StackLayout, AutoSizeResizesControl)
{
    ui::Window control(Size(), Colour::White);
    auto layout = std::make_unique<StackLayout>();
    control.set_layout(std::move(layout));

    ASSERT_EQ(control.size(), Size());
    auto first_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));
    ASSERT_EQ(control.size(), Size(10, 10));
}

// Tests that when manual size mode is on the control is not resized.
TEST(StackLayout, ManualSizeDoesntResizeControl)
{
    ui::Window control(Size(), Colour::White);
    auto layout = std::make_unique<StackLayout>(0, StackLayout::Direction::Horizontal, SizeMode::Manual);
    control.set_layout(std::move(layout));

    ASSERT_EQ(control.size(), Size());
    auto first_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));
    ASSERT_EQ(control.size(), Size());
}

// Tests that the control is only expanded in the dimensions specified.
TEST(StackLayout, AutoSizeDimensionWidth)
{
    ui::Window control(Size(), Colour::White);
    auto layout = std::make_unique<StackLayout>();
    layout->set_auto_size_dimension(SizeDimension::Width);
    control.set_layout(std::move(layout));

    ASSERT_EQ(control.size(), Size());
    auto first_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));
    ASSERT_EQ(control.size(), Size(10, 0));
}

// Tests that the control is only expanded in the dimensions specified.
TEST(StackLayout, AutoSizeDimensionHeight)
{
    ui::Window control(Size(), Colour::White);
    auto layout = std::make_unique<StackLayout>();
    layout->set_auto_size_dimension(SizeDimension::Height);
    control.set_layout(std::move(layout));

    ASSERT_EQ(control.size(), Size());
    auto first_element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));
    ASSERT_EQ(control.size(), Size(0, 10));
}

// Tests that a margin is applied before the first element.
TEST(StackLayout, Margin)
{
    ui::Window control(Size(), Colour::White);
    auto layout = std::make_unique<StackLayout>();
    layout->set_margin(Size(0, 5));
    control.set_layout(std::move(layout));

    auto element = control.add_child(std::make_unique<ui::Window>(Size(10, 10), Colour::White));
    ASSERT_EQ(element->position(), Point(0, 5));
}
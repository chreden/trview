#include <trview.app/UI/Bubble.h>

using namespace trview;
using namespace trview::ui;

TEST(Bubble, ShowMakesBubbleVisible)
{
    ui::Window control(Size(), Colour::White);
    Bubble bubble(control);

    auto bubble_control = control.find<ui::Control>(Bubble::Names::Bubble);
    ASSERT_NE(bubble_control, nullptr);
    ASSERT_FALSE(bubble_control->visible());

    bubble.show(Point());
    ASSERT_TRUE(bubble_control->visible());
}

TEST(Bubble, BubbleFadesToInvisibleOverTime)
{
    ui::Window control(Size(), Colour::White);
    Bubble bubble(control);

    auto bubble_control = control.find<ui::Label>(Bubble::Names::Bubble);
    ASSERT_NE(bubble_control, nullptr);
    ASSERT_FALSE(bubble_control->visible());

    bubble.show(Point());
    ASSERT_TRUE(bubble_control->visible());
    ASSERT_FLOAT_EQ(bubble_control->background_colour().a, 1.0f);
    ASSERT_FLOAT_EQ(bubble_control->text_colour().a, 1.0f);

    control.update(Bubble::FadeTime * 0.5f);
    ASSERT_FLOAT_EQ(bubble_control->background_colour().a, 0.5f);
    ASSERT_FLOAT_EQ(bubble_control->text_colour().a, 0.5f);

    control.update(Bubble::FadeTime * 0.5f);
    ASSERT_FALSE(bubble_control->visible());
}
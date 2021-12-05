#include <gtest/gtest.h>
#include <trview.ui/Scrollbar.h>
#include <optional>

using namespace trview;
using namespace trview::ui;

TEST(Scrollbar, Range)
{
    Scrollbar scrollbar(Size(10, 100), Colour::White);
    auto blob = scrollbar.find<Control>(Scrollbar::Names::blob);
    ASSERT_NE(blob, nullptr);

    scrollbar.set_range(0, 10, 10);
    ASSERT_FALSE(blob->visible());

    scrollbar.set_range(0, 5, 10);
    ASSERT_TRUE(blob->visible());
    ASSERT_EQ(blob->size().height, (scrollbar.size().height * 0.5f) - 2);
}

TEST(Scrollbar, ScrollEventRaised)
{
    Scrollbar scrollbar(Size(10, 100), Colour::White);

    std::optional<float> raised;
    auto token = scrollbar.on_scroll += [&](float value)
    {
        raised = value;
    };

    scrollbar.clicked(Point(0, 100));

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 1.0f);
}

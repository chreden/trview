#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <trview.ui/Slider.h>
#include <trview.ui/json.h>
#include <optional>
#include <trview.ui/Mocks/Input/IInput.h>

using namespace trview;
using namespace trview::ui;
using namespace trview::ui::mocks;

TEST(Slider, LoadFromJson)
{
    const std::string json = "{ \"type\":\"slider\" }";
    auto slider = ui::load_from_json(json);
    ASSERT_NE(slider, nullptr);
    ASSERT_NE(dynamic_cast<Slider*>(slider.get()), nullptr);
}

TEST(Slider, ValueChangedRaised)
{
    Slider slider(Size(100, 10));

    std::optional<float> raised;
    auto token = slider.on_value_changed += [&](float value)
    {
        raised = value;
    };

    slider.clicked(Point(100, 0));

    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 1.0f);
}

TEST(Slider, Value)
{
    Slider slider(Size(100, 10));
    ASSERT_EQ(slider.value(), 0.0f);

    auto blob = slider.find<Control>(Slider::Names::blob);
    ASSERT_NE(blob, nullptr);
    ASSERT_EQ(blob->position().x, 5.0f);

    slider.set_value(1.0f);
    ASSERT_EQ(slider.value(), 1.0f);
    ASSERT_EQ(blob->position().x, 85.0f);
}

TEST(Slider, ClickAndDrag)
{
    Slider slider(Size(100, 10));
    ASSERT_EQ(slider.value(), 0.0f);

    MockInput input;
    EXPECT_CALL(input, focus_control).WillRepeatedly(testing::Return(&slider));

    slider.set_input(&input);
    slider.mouse_down(Point(0, 0));
    slider.move(Point(100, 0));
    slider.mouse_up(Point(100, 0));

    ASSERT_EQ(slider.value(), 1.0f);
}


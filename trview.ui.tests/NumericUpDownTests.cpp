#include <trview.ui/NumericUpDown.h>
#include <trview.ui/JsonLoader.h>
#include <trview.ui/Button.h>
#include <trview.ui/Label.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

TEST(NumericUpDown, LoadFromJson)
{
    const std::string json = "{\"type\":\"numericupdown\",\"min_value\":10,\"max_value\":90,\"value\":50}";
    auto control = JsonLoader(std::make_shared<MockShell>()).load_from_json(json);
    ASSERT_NE(control, nullptr);
    auto numeric_up_down = dynamic_cast<NumericUpDown*>(control.get());
    ASSERT_NE(numeric_up_down, nullptr);
    ASSERT_EQ(numeric_up_down->min_value(), 10);
    ASSERT_EQ(numeric_up_down->max_value(), 90);
    ASSERT_EQ(numeric_up_down->value(), 50);
}

TEST(NumericUpDown, ValueDisplayChanges)
{
    NumericUpDown control(Size(), Colour::Transparent, 0, 100);

    auto up = control.find<Button>(NumericUpDown::Names::up);
    ASSERT_NE(up, nullptr);

    auto down = control.find<Button>(NumericUpDown::Names::down);
    ASSERT_NE(down, nullptr);

    auto label = control.find<Label>(NumericUpDown::Names::value);
    ASSERT_NE(label, nullptr);
    ASSERT_EQ(label->text(), L"0");

    up->clicked(Point());
    ASSERT_EQ(label->text(), L"1");

    down->clicked(Point());
    ASSERT_EQ(label->text(), L"0");
}

TEST(NumericUpDown, ValueChangedEventRaised)
{
    NumericUpDown control(Size(), Colour::Transparent, 0, 100);

    auto up = control.find<Button>(NumericUpDown::Names::up);
    ASSERT_NE(up, nullptr);

    std::optional<int32_t> raised;
    auto token = control.on_value_changed += [&](int32_t value)
    {
        raised = value;
    };

    up->clicked(Point());
    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 1);
    ASSERT_EQ(control.value(), 1);

    raised.reset();

    auto down = control.find<Button>(NumericUpDown::Names::down);
    ASSERT_NE(down, nullptr);

    down->clicked(Point());
    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(raised.value(), 0);
    ASSERT_EQ(control.value(), 0);
}

TEST(NumericUpDown, MinValueApplied)
{
    NumericUpDown control(Size(), Colour::Transparent, 0, 100);

    std::optional<int32_t> raised;
    auto token = control.on_value_changed += [&](int32_t value)
    {
        raised = value;
    };

    auto down = control.find<Button>(NumericUpDown::Names::down);
    ASSERT_NE(down, nullptr);

    down->clicked(Point());
    ASSERT_FALSE(raised.has_value());
    ASSERT_EQ(control.value(), 0);
}

TEST(NumericUpDown, MaxValueApplied)
{
    NumericUpDown control(Size(), Colour::Transparent, 0, 100);
    control.set_value(100);

    std::optional<int32_t> raised;
    auto token = control.on_value_changed += [&](int32_t value)
    {
        raised = value;
    };

    auto up = control.find<Button>(NumericUpDown::Names::up);
    ASSERT_NE(up, nullptr);

    up->clicked(Point());
    ASSERT_FALSE(raised.has_value());
    ASSERT_EQ(control.value(), 100);
}

TEST(NumericUpDown, SetValue)
{
    NumericUpDown control(Size(), Colour::Transparent, 0, 100);
    ASSERT_EQ(control.value(), 0);

    auto label = control.find<Label>(NumericUpDown::Names::value);
    ASSERT_EQ(label->text(), L"0");

    control.set_value(5);
    ASSERT_EQ(control.value(), 5);
    ASSERT_EQ(label->text(), L"5");
}

TEST(NumericUpDown, SetMaximum)
{
    NumericUpDown control(Size(), Colour::Transparent, 0, 100);
    control.set_value(100);
    ASSERT_EQ(control.value(), 100);
    ASSERT_EQ(control.max_value(), 100);

    control.set_maximum(50);
    ASSERT_EQ(control.value(), 50);
    ASSERT_EQ(control.max_value(), 50);

    auto up = control.find<Button>(NumericUpDown::Names::up);
    ASSERT_NE(up, nullptr);
    up->clicked(Point());

    ASSERT_EQ(control.value(), 50);
}

TEST(NumericUpDown, SetMinimum)
{
    NumericUpDown control(Size(), Colour::Transparent, 0, 100);
    ASSERT_EQ(control.min_value(), 0);

    control.set_minimum(10);
    ASSERT_EQ(control.value(), 10);
    ASSERT_EQ(control.min_value(), 10);

    auto down = control.find<Button>(NumericUpDown::Names::down);
    ASSERT_NE(down, nullptr);
    down->clicked(Point());

    ASSERT_EQ(control.value(), 10);
}

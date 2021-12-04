#include "gtest/gtest.h"
#include <trview.ui/NumericUpDown.h>
#include <trview.ui/json.h>

using namespace trview;
using namespace trview::ui;

TEST(NumericUpDown, LoadFromJson)
{
    const std::string json = "{\"type\":\"numericupdown\",\"min_value\":10,\"max_value\":90,\"value\":50}";
    auto control = ui::load_from_json(json);
    ASSERT_NE(control, nullptr);
    auto numeric_up_down = dynamic_cast<NumericUpDown*>(control.get());
    ASSERT_NE(numeric_up_down, nullptr);
    ASSERT_EQ(numeric_up_down->min_value(), 10);
    ASSERT_EQ(numeric_up_down->max_value(), 90);
    ASSERT_EQ(numeric_up_down->value(), 50);
}

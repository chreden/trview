#include "gtest/gtest.h"
#include <trview.ui/GroupBox.h>
#include <trview.ui/json.h>

using namespace trview;
using namespace trview::ui;

TEST(GroupBox, LoadFromJson)
{
    const std::string json = "{\"type\":\"groupbox\",\"text\":\"Test\",\"border_colour\":\"red\"}";
    auto control = ui::load_from_json(json);
    ASSERT_NE(control, nullptr);
    auto group_box = dynamic_cast<GroupBox*>(control.get());
    ASSERT_NE(group_box, nullptr);
    ASSERT_EQ(group_box->title(), L"Test");
    ASSERT_EQ(group_box->border_colour(), Colour::Red);
}

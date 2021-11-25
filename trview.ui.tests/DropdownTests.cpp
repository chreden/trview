#include "gtest/gtest.h"
#include <trview.ui/Dropdown.h>
#include <trview.ui/json.h>

using namespace trview;
using namespace trview::ui;

TEST(Dropdown, LoadFromJson)
{
    const std::string json = "{\"type\":\"dropdown\",\"text\":\"Test\",\"text_colour\":\"red\",\"text_background_colour\":\"blue\",\"values\":[\"One\",{\"name\":\"Two\",\"foreground_colour\":\"red\",\"background_colour\":\"white\"},\"Three\"],\"selected_value\":\"Two\"}";
    auto control = ui::load_from_json(json);
    ASSERT_NE(control, nullptr);
    auto dropdown = dynamic_cast<Dropdown*>(control.get());
    ASSERT_NE(dropdown, nullptr);

    const std::vector<Dropdown::Value> expected
    {
        { L"One" },
        { L"Two", Colour::Red, Colour::White },
        { L"Three" }
    };
    ASSERT_EQ(dropdown->values(), expected);
    ASSERT_EQ(dropdown->selected_value(), L"Two");
    ASSERT_EQ(dropdown->text_colour(), Colour::Red);
    ASSERT_EQ(dropdown->text_background_colour(), Colour::Blue);
}

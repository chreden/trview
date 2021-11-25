#include "gtest/gtest.h"
#include <trview.ui/Slider.h>
#include <trview.ui/json.h>

using namespace trview;
using namespace trview::ui;

TEST(Slider, LoadFromJson)
{
    const std::string json = "{ \"type\":\"slider\" }";
    auto slider = ui::load_from_json(json);
    ASSERT_NE(slider, nullptr);
    ASSERT_NE(dynamic_cast<Slider*>(slider.get()), nullptr);
}

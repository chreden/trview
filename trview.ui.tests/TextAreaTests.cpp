#include "gtest/gtest.h"
#include <trview.ui/TextArea.h>
#include <trview.ui/json.h>

using namespace trview;
using namespace trview::ui;

TEST(TextArea, LoadFromJson)
{
    const std::string json = "{\"type\":\"textarea\",\"show_scrollbar\":false,\"text_alignment\":\"centre\",\"read_only\":true,\"line_mode\":\"single\",\"text\":\"Test\",\"size\":{\"width\":100,\"height\":100}}";
    auto control = ui::load_from_json(json);
    ASSERT_NE(control, nullptr);
    auto textarea = dynamic_cast<TextArea*>(control.get());
    ASSERT_NE(textarea, nullptr);
    ASSERT_EQ(textarea->scrollbar_visible(), false);
    ASSERT_EQ(textarea->text_alignment(), graphics::TextAlignment::Centre);
    ASSERT_EQ(textarea->read_only(), true);
    ASSERT_EQ(textarea->line_mode(), TextArea::Mode::SingleLine);
    ASSERT_EQ(textarea->text(), L"Test");
}

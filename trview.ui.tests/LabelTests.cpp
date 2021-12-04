#include "gtest/gtest.h"
#include <trview.ui/Label.h>
#include <trview.ui/json.h>

using namespace trview;
using namespace trview::ui;

TEST(Label, LoadFromJson)
{
    const std::string json = "{\"type\":\"label\",\"text\":\"Test\",\"size_mode\":\"auto\",\"text_size\":10,\"text_alignment\":\"right\",\"paragraph_alignment\":\"centre\"}";
    auto control = ui::load_from_json(json);
    ASSERT_NE(control, nullptr);

    auto label = dynamic_cast<Label*>(control.get());
    ASSERT_NE(label, nullptr);
    ASSERT_EQ(label->text(), L"Test");
    ASSERT_EQ(label->size_mode(), SizeMode::Auto);
    ASSERT_EQ(label->text_size(), 10);
    ASSERT_EQ(label->text_alignment(), graphics::TextAlignment::Right);
    ASSERT_EQ(label->paragraph_alignment(), graphics::ParagraphAlignment::Centre);
}

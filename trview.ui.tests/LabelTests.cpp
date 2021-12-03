#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include <trview.ui/Label.h>
#include <trview.ui/json.h>
#include <trview.ui/Mocks/IFontMeasurer.h>
#include <optional>

using namespace trview;
using namespace trview::ui;
using namespace trview::ui::mocks;
using namespace trview::graphics;

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

TEST(Label, ConstructorParameters)
{
    Label label(Size(), Colour::White, L"Text", 8, TextAlignment::Centre, ParagraphAlignment::Centre, SizeMode::Auto);
    ASSERT_EQ(label.text(), L"Text");
    ASSERT_EQ(label.text_size(), 8);
    ASSERT_EQ(label.text_alignment(), TextAlignment::Centre);
    ASSERT_EQ(label.paragraph_alignment(), ParagraphAlignment::Centre);
    ASSERT_EQ(label.size_mode(), SizeMode::Auto);
}

TEST(Label, SetText)
{
    Label label(Size(), Colour::White, L"", 8);
    ASSERT_EQ(label.text(), L"");
    label.set_text(L"Test");
    ASSERT_EQ(label.text(), L"Test");
}

TEST(Label, TextChangedEventRaised)
{
    Label label(Size(), Colour::White, L"", 8);

    std::optional<std::wstring> raised;
    auto token = label.on_text_changed += [&](const auto& text)
    {
        raised = text;
    };

    label.set_text(L"Test");
    ASSERT_TRUE(raised.has_value());
    ASSERT_EQ(label.text(), L"Test"); 
    ASSERT_EQ(raised.value(), L"Test");
}

TEST(Label, MeasurerCalled)
{
    Label label(Size(), Colour::White, L"", 8);

    MockFontMeasurer measurer;
    EXPECT_CALL(measurer, measure(std::wstring(L"Test"))).Times(1).WillOnce(testing::Return(Size(10, 20)));

    label.set_measurer(&measurer);
    auto size = label.measure_text(L"Test");
    ASSERT_EQ(size, Size(10, 20));
}

TEST(Label, SetFont)
{
    Label label(Size(), Colour::White, L"", 8);
    label.set_font("Test font");
    ASSERT_EQ(label.font(), "Test font");
}

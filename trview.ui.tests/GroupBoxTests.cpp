#include <gmock/gmock.h>
#include "gtest/gtest.h"
#include <trview.ui/GroupBox.h>
#include <trview.ui/JsonLoader.h>
#include <trview.common/Mocks/Windows/IShell.h>
#include <trview.ui/Label.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;

TEST(GroupBox, LoadFromJson)
{
    const std::string json = "{\"type\":\"groupbox\",\"text\":\"Test\",\"border_colour\":\"red\"}";
    auto control = JsonLoader(std::make_shared<MockShell>()).load_from_json(json);
    ASSERT_NE(control, nullptr);
    auto group_box = dynamic_cast<GroupBox*>(control.get());
    ASSERT_NE(group_box, nullptr);
    ASSERT_EQ(group_box->title(), L"Test");
    ASSERT_EQ(group_box->border_colour(), Colour::Red);
}

TEST(GroupBox, Title)
{
    GroupBox box(Size(100, 100), Colour::White, Colour::White, L"Test");
    auto title = box.find<Label>(GroupBox::Names::title);
    ASSERT_NE(title, nullptr);
    ASSERT_EQ(title->text(), L"Test");
    box.set_title(L"Test 2");
    ASSERT_EQ(title->text(), L"Test 2");
}

#include "stdafx.h"
#include <trview.app/UI/CameraPosition.h>

using namespace trview;
using namespace trview::ui;
using namespace DirectX::SimpleMath;
using testing::HasSubstr;

/// Tests that the position event is raised when the coordinates are changed.
TEST(CameraPosition, PositionEventRaised)
{
    ui::Window window(Point(), Size(100, 100), Colour::Transparent);

    int times_called = 0;
    Vector3 new_position;

    auto subject = CameraPosition(window);
    auto token = subject.on_position_changed += [&times_called, &new_position](const auto& position) 
    {
        ++times_called;
        new_position = position;
    };

    auto area_x = window.find<TextArea>("X");
    area_x->gained_focus();
    area_x->set_text(L"1024");
    area_x->key_char(0xD);

    auto area_y = window.find<TextArea>("Y");
    area_y->gained_focus();
    area_y->set_text(L"2048");
    area_y->key_char(0xD);

    auto area_z = window.find<TextArea>("Z");
    area_z->gained_focus();
    area_z->set_text(L"3072");
    area_z->key_char(0xD);

    ASSERT_EQ(times_called, 3);
    // The position is scaled by diving by 1024.
    ASSERT_EQ(new_position.x, 1.0f);
    ASSERT_EQ(new_position.y, 2.0f);
    ASSERT_EQ(new_position.z, 3.0f);
}

/// Tests that the coordinate labels are updated to have the correct position values.
TEST(CameraPosition, CoordinatesUpdated)
{
    ui::Window window(Point(), Size(100, 100), Colour::Transparent);
    auto subject = CameraPosition(window);

    auto area_x = window.find<TextArea>("X");
    auto area_y = window.find<TextArea>("Y");
    auto area_z = window.find<TextArea>("Z");

    subject.set_position(Vector3(1, 2, 3));

    EXPECT_THAT(area_x->text(), HasSubstr(L"1024.000000"));
    EXPECT_THAT(area_y->text(), HasSubstr(L"2048.000000"));
    EXPECT_THAT(area_z->text(), HasSubstr(L"3072.000000"));
}
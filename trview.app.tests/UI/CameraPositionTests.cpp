#include <trview.app/UI/CameraPosition.h>
#include <trview.ui/JsonLoader.h>
#include <trview.common/Mocks/Windows/IShell.h>

using namespace trview;
using namespace trview::mocks;
using namespace trview::ui;
using namespace DirectX::SimpleMath;
using testing::HasSubstr;

/*
/// Tests that the position event is raised when the coordinates are changed.
TEST(CameraPosition, PositionEventRaised)
{
    ui::Window window(Point(), Size(100, 100), Colour::Transparent);

    int times_called = 0;
    Vector3 new_position;

    auto subject = CameraPosition(window, JsonLoader(std::make_shared<MockShell>()));
    auto token = subject.on_position_changed += [&times_called, &new_position](const auto& position) 
    {
        ++times_called;
        new_position = position;
    };

    auto area_x = window.find<TextArea>(CameraPosition::Names::x);
    area_x->gained_focus();
    area_x->set_text(L"1024");
    area_x->key_char(0xD);

    auto area_y = window.find<TextArea>(CameraPosition::Names::y);
    area_y->gained_focus();
    area_y->set_text(L"2048");
    area_y->key_char(0xD);

    auto area_z = window.find<TextArea>(CameraPosition::Names::z);
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
    auto subject = CameraPosition(window, JsonLoader(std::make_shared<MockShell>()));

    auto area_x = window.find<TextArea>(CameraPosition::Names::x);
    auto area_y = window.find<TextArea>(CameraPosition::Names::y);
    auto area_z = window.find<TextArea>(CameraPosition::Names::z);

    subject.set_position(Vector3(1, 2, 3));

    EXPECT_THAT(area_x->text(), HasSubstr(L"1024.0000"));
    EXPECT_THAT(area_y->text(), HasSubstr(L"2048.0000"));
    EXPECT_THAT(area_z->text(), HasSubstr(L"3072.0000"));
}

TEST(CameraPosition, RotationEventRaised)
{
    ui::Window window(Point(), Size(100, 100), Colour::Transparent);

    int times_called = 0;
    float new_yaw = 0;
    float new_pitch = 0;

    auto subject = CameraPosition(window, JsonLoader(std::make_shared<MockShell>()));
    auto token = subject.on_rotation_changed += [&times_called, &new_yaw, &new_pitch](float yaw, float pitch)
    {
        ++times_called;
        new_yaw = yaw;
        new_pitch = pitch;
    };

    auto area_yaw = window.find<TextArea>(CameraPosition::Names::yaw);
    area_yaw->gained_focus();
    area_yaw->set_text(L"90");
    area_yaw->key_char(0xD);

    auto area_pitch = window.find<TextArea>(CameraPosition::Names::pitch);
    area_pitch->gained_focus();
    area_pitch->set_text(L"180");
    area_pitch->key_char(0xD);

    ASSERT_EQ(times_called, 2);
    ASSERT_FLOAT_EQ(new_yaw, 1.5707964);
    ASSERT_FLOAT_EQ(new_pitch, 3.1415927);
}

TEST(CameraPosition, RotationUpdated)
{
    ui::Window window(Point(), Size(100, 100), Colour::Transparent);
    auto subject = CameraPosition(window, JsonLoader(std::make_shared<MockShell>()));

    const auto pi = 3.1415926535897932384626433832795f;

    auto area_yaw = window.find<TextArea>(CameraPosition::Names::yaw);
    auto area_pitch = window.find<TextArea>(CameraPosition::Names::pitch);

    subject.set_rotation(pi, pi * 0.5f);

    EXPECT_THAT(area_yaw->text(), HasSubstr(L"180"));
    EXPECT_THAT(area_pitch->text(), HasSubstr(L"90"));
}

TEST(CameraPosition, RotationShowRadians)
{
    ui::Window window(Point(), Size(100, 100), Colour::Transparent);
    auto subject = CameraPosition(window, JsonLoader(std::make_shared<MockShell>()));
    subject.set_display_degrees(false);

    const auto pi = 3.1415926535897932384626433832795f;

    auto area_yaw = window.find<TextArea>(CameraPosition::Names::yaw);
    auto area_pitch = window.find<TextArea>(CameraPosition::Names::pitch);

    subject.set_rotation(pi, pi * 0.5f);

    EXPECT_THAT(area_yaw->text(), HasSubstr(L"3.1416"));
    EXPECT_THAT(area_pitch->text(), HasSubstr(L"1.5708"));
}

TEST(CameraPosition, RotationNotUpdatedWithInvalidValues)
{
    ui::Window window(Point(), Size(100, 100), Colour::Transparent);
    auto subject = CameraPosition(window, JsonLoader(std::make_shared<MockShell>()));

    auto area_yaw = window.find<TextArea>(CameraPosition::Names::yaw);
    area_yaw->gained_focus();
    area_yaw->set_text(L"inf");
    area_yaw->key_char(0xD);

    auto area_pitch = window.find<TextArea>(CameraPosition::Names::pitch);
    area_pitch->gained_focus();
    area_pitch->set_text(L"nan");
    area_pitch->key_char(0xD);

    bool raised = false;
    auto token = subject.on_rotation_changed += [&](auto&&...)
    {
        raised = true;
    };

    ASSERT_FALSE(raised);
}

TEST(CameraPosition, CoordinatesNotUpdatedWithInvalidValues)
{
    ui::Window window(Point(), Size(100, 100), Colour::Transparent);
    auto subject = CameraPosition(window, JsonLoader(std::make_shared<MockShell>()));

    auto area_x = window.find<TextArea>(CameraPosition::Names::x);
    area_x->gained_focus();
    area_x->set_text(L"inf");
    area_x->key_char(0xD);

    auto area_y = window.find<TextArea>(CameraPosition::Names::y);
    area_y->gained_focus();
    area_y->set_text(L"nan");
    area_y->key_char(0xD);

    auto area_z = window.find<TextArea>(CameraPosition::Names::z);
    area_z->gained_focus();
    area_z->set_text(L"nan");
    area_z->key_char(0xD);

    bool raised = false;
    auto token = subject.on_rotation_changed += [&](auto&&...)
    {
        raised = true;
    };

    ASSERT_FALSE(raised);
}
*/
#include <trview.app/UI/CameraPosition.h>
#include "TestImgui.h"

using namespace trview;
using namespace trview::tests;
using namespace DirectX::SimpleMath;
using testing::HasSubstr;

/// Tests that the position event is raised when the coordinates are changed.
TEST(CameraPosition, PositionEventRaised)
{
    CameraPosition subject;
    TestImgui imgui([&]() { subject.render(); });

    int times_called = 0;
    Vector3 new_position;

    auto token = subject.on_position_changed += [&times_called, &new_position](const auto& position) 
    {
        ++times_called;
        new_position = position;
    };

    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::x));
    imgui.enter_text("1024");
    imgui.press_key(ImGuiKey_Enter);

    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::y));
    imgui.enter_text("2048");
    imgui.press_key(ImGuiKey_Enter);
    
    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::z));
    imgui.enter_text("3072");
    imgui.press_key(ImGuiKey_Enter);

    ASSERT_EQ(times_called, 3);
    // The position is scaled by diving by 1024.
    ASSERT_EQ(new_position.x, 1.0f);
    ASSERT_EQ(new_position.y, 2.0f);
    ASSERT_EQ(new_position.z, 3.0f);
}

/// Tests that the coordinate labels are updated to have the correct position values.
TEST(CameraPosition, CoordinatesUpdated)
{
    CameraPosition subject;
    TestImgui imgui([&]() { subject.render(); });

    subject.set_position(Vector3(1, 2, 3));
    imgui.render();

    EXPECT_THAT(imgui.item_text(imgui.id("Camera Position").id(CameraPosition::Names::x)), HasSubstr("1024"));
    EXPECT_THAT(imgui.item_text(imgui.id("Camera Position").id(CameraPosition::Names::y)), HasSubstr("2048"));
    EXPECT_THAT(imgui.item_text(imgui.id("Camera Position").id(CameraPosition::Names::z)), HasSubstr("3072"));
}


TEST(CameraPosition, RotationEventRaised)
{
    CameraPosition subject;
    TestImgui imgui([&]() { subject.render(); });

    int times_called = 0;
    float new_yaw = 0;
    float new_pitch = 0;

    auto token = subject.on_rotation_changed += [&times_called, &new_yaw, &new_pitch](float yaw, float pitch)
    {
        ++times_called;
        new_yaw = yaw;
        new_pitch = pitch;
    };

    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::yaw));
    imgui.enter_text("90");
    imgui.press_key(ImGuiKey_Enter);
    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::pitch));
    imgui.enter_text("180");
    imgui.press_key(ImGuiKey_Enter);

    ASSERT_EQ(times_called, 2);
    ASSERT_FLOAT_EQ(new_yaw, 1.5707964f);
    ASSERT_FLOAT_EQ(new_pitch, 3.1415927f);
}

TEST(CameraPosition, RotationUpdated)
{
    CameraPosition subject;
    TestImgui imgui([&]() { subject.render(); });

    const auto pi = 3.1415926535897932384626433832795f;
    subject.set_rotation(pi, pi * 0.5f);
    imgui.render();

    EXPECT_THAT(imgui.item_text(imgui.id("Camera Position").id(CameraPosition::Names::yaw)), HasSubstr("180"));
    EXPECT_THAT(imgui.item_text(imgui.id("Camera Position").id(CameraPosition::Names::pitch)), HasSubstr("90"));
}

TEST(CameraPosition, RotationShowRadians)
{
    CameraPosition subject;
    TestImgui imgui([&]() { subject.render(); });
    subject.set_display_degrees(false);

    const auto pi = 3.1415926535897932384626433832795f;

    subject.set_rotation(pi, pi * 0.5f);
    imgui.render();

    EXPECT_THAT(imgui.item_text(imgui.id("Camera Position").id(CameraPosition::Names::yaw)), HasSubstr("3.1416"));
    EXPECT_THAT(imgui.item_text(imgui.id("Camera Position").id(CameraPosition::Names::pitch)), HasSubstr("1.5708"));
}

TEST(CameraPosition, RotationNotUpdatedWithInvalidValues)
{
    CameraPosition subject;
    TestImgui imgui([&]() { subject.render(); });

    bool raised = false;
    auto token = subject.on_rotation_changed += [&](auto&&...)
    {
        raised = true;
    };

    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::yaw));
    imgui.enter_text("inf");
    imgui.press_key(ImGuiKey_Enter);
    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::pitch));
    imgui.enter_text("nan");
    imgui.press_key(ImGuiKey_Enter);

    ASSERT_FALSE(raised);
}

TEST(CameraPosition, CoordinatesNotUpdatedWithInvalidValues)
{
    CameraPosition subject;
    TestImgui imgui([&]() { subject.render(); });

    bool raised = false;
    auto token = subject.on_rotation_changed += [&](auto&&...)
    {
        raised = true;
    };

    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::x));
    imgui.enter_text("inf");
    imgui.press_key(ImGuiKey_Enter);
    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::y));
    imgui.enter_text("nan");
    imgui.press_key(ImGuiKey_Enter);
    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::z));
    imgui.enter_text("nan");
    imgui.press_key(ImGuiKey_Enter);

    ASSERT_FALSE(raised);
}

TEST(CameraPosition, RotationsCorrectlyConverted)
{
    CameraPosition subject;
    subject.set_rotation(1, 2);

    TestImgui imgui([&]() { subject.render(); });

    std::optional<std::tuple<float, float>> value;
    auto token = subject.on_rotation_changed += [&](auto yaw, auto pitch)
    {
        value = { yaw, pitch };
    };

    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::yaw));
    imgui.enter_text("90");
    imgui.press_key(ImGuiKey_Enter);

    ASSERT_TRUE(value.has_value());
    ASSERT_FLOAT_EQ(std::get<0>(value.value()), 1.570796326f);
    ASSERT_FLOAT_EQ(std::get<1>(value.value()), 2.0f);
}

TEST(CameraPosition, YawRemovesNegatives)
{
    CameraPosition subject;
    subject.set_rotation(-1.5707963267f, 1);

    TestImgui imgui([&]() { subject.render(); });

    std::optional<std::tuple<float, float>> value;
    auto token = subject.on_rotation_changed += [&](auto yaw, auto pitch)
    {
        value = { yaw, pitch };
    };

    imgui.click_element(imgui.id("Camera Position").id(CameraPosition::Names::pitch));
    imgui.enter_text("0");
    imgui.press_key(ImGuiKey_Enter);

    ASSERT_TRUE(value);
    ASSERT_FLOAT_EQ(std::get<0>(value.value()), 4.71238898f);
    ASSERT_FLOAT_EQ(std::get<1>(value.value()), 0);
}


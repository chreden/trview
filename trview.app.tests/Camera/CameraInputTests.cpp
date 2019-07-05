#include "gtest/gtest.h"
#include <trview.app/Camera/CameraInput.h>
#include <trview.common/TokenStore.h>
#include <set>

using namespace trview;

/// Tests that when the appropriate shortcut key is pressed the event
/// for entering free mode is raised.
TEST(CameraInput, EnterFreeMode)
{
    CameraInput subject;

    std::optional<CameraMode> mode;
    auto token = subject.on_mode_change += [&mode](CameraMode new_mode)
    {
        mode = new_mode;
    };

    subject.key_down(L'F', false);

    ASSERT_TRUE(mode.has_value());
    ASSERT_EQ(CameraMode::Free, mode.value());
}

/// Tests that when the appropriate shortcut key is pressed the event
/// for entering axis mode is raised.
TEST(CameraInput, EnterAxisMode)
{
    CameraInput subject;

    std::optional<CameraMode> mode;
    auto token = subject.on_mode_change += [&mode](CameraMode new_mode)
    {
        mode = new_mode;
    };

    subject.key_down(L'X', false);

    ASSERT_TRUE(mode.has_value());
    ASSERT_EQ(CameraMode::Axis, mode.value());
}

/// Tests that when the appropriate shortcut key is pressed the event
/// for entering orbit mode is raised.
TEST(CameraInput, EnterOrbitMode)
{
    CameraInput subject;

    std::optional<CameraMode> mode;
    auto token = subject.on_mode_change += [&mode](CameraMode new_mode)
    {
        mode = new_mode;
    };

    subject.key_down(L'O', false);

    ASSERT_TRUE(mode.has_value());
    ASSERT_EQ(CameraMode::Orbit, mode.value());
}

/// Tests that when the mouse is scrolled down the zoom event is raised.
TEST(CameraInput, ZoomOut)
{
    CameraInput subject;

    std::optional<float> zoom;
    auto token = subject.on_zoom += [&zoom](float new_zoom)
    {
        zoom = new_zoom;
    };

    subject.mouse_scroll(200);

    ASSERT_TRUE(zoom.has_value());
    ASSERT_LT(zoom.value(), 0.0f);
}

/// Tests that when the mouse is scrolled up tzoom event is raised.
TEST(CameraInput, ZoomIn)
{
    CameraInput subject;

    std::optional<float> zoom;
    auto token = subject.on_zoom += [&zoom](float new_zoom)
    {
        zoom = new_zoom;
    };

    subject.mouse_scroll(-200);

    ASSERT_TRUE(zoom.has_value());
    ASSERT_GT(zoom.value(), 0.0f);
}

/// Tests that right clicking and moving the mouse causes the rotation
/// event to be raised.
TEST(CameraInput, Rotation)
{
    CameraInput subject;

    std::optional<std::tuple<float, float>> rotation;
    auto token = subject.on_rotate += [&rotation](float x, float y)
    {
        rotation = { x, y };
    };

    subject.mouse_down(input::Mouse::Button::Right);
    subject.mouse_move(100, 200);
    subject.mouse_up(input::Mouse::Button::Right);

    ASSERT_TRUE(rotation.has_value());
    ASSERT_EQ(100.0f, std::get<0>(rotation.value()));
    ASSERT_EQ(200.0f, std::get<1>(rotation.value()));
}

/// Tests that giving the appropriate input to the camera input results in the
/// correct movement vector.
TEST(CameraInput, Movement)
{
    using namespace DirectX::SimpleMath;

    CameraInput subject;

    ASSERT_EQ(Vector3::Zero, subject.movement());

    subject.key_down(L'W', false);
    ASSERT_EQ(Vector3(0, 0, 1), subject.movement());

    subject.key_down(L'A', false);
    ASSERT_EQ(Vector3(-1, 0, 1), subject.movement());

    subject.key_up(L'A');
    ASSERT_EQ(Vector3(0, 0, 1), subject.movement());

    subject.key_down(L'D', false);
    ASSERT_EQ(Vector3(1, 0, 1), subject.movement());

    subject.key_up(L'W');
    ASSERT_EQ(Vector3(1, 0, 0), subject.movement());

    subject.key_down(L'S', false);
    ASSERT_EQ(Vector3(1, 0, -1), subject.movement());

    subject.key_up(L'S');
    ASSERT_EQ(Vector3(1, 0, 0), subject.movement());

    subject.key_up(L'D');
    ASSERT_EQ(Vector3::Zero, subject.movement());
}

/// Tests that if the control key is pressed nothing is done.
TEST(CameraInput, ControlBlocks)
{
    using namespace DirectX::SimpleMath;

    CameraInput subject;

    std::set<CameraMode> modes;
    auto token = subject.on_mode_change += [&modes](CameraMode mode)
    {
        modes.insert(mode);
    };

    subject.key_down('F', true);
    subject.key_down('O', true);
    subject.key_down('X', true);
    ASSERT_TRUE(modes.empty());

    subject.key_down('W', true);
    subject.key_down('A', true);
    ASSERT_EQ(Vector3::Zero, subject.movement());

    subject.key_down('S', true);
    subject.key_down('D', true);
    ASSERT_EQ(Vector3::Zero, subject.movement());
}
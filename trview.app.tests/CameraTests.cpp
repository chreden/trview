#include <trview.app/Camera/Camera.h>
#include <trview.tests.common/Event.h>

using namespace trview;
using namespace DirectX::SimpleMath;

/// Tests that when the camera is in alignment mode it moves relative the world axis.
TEST(Camera, Alignment)
{
    Camera camera(Size(100, 100));

    // Set the camera to be slightly pitched, so that moving up in camera mode
    // would move the camera away from 0,0 on the x/z axis.
    camera.set_rotation_pitch(0.45f);

    // Ensure the camera is in axis mode.
    camera.set_mode(ICamera::Mode::Axis);
    camera.set_position(Vector3::Zero);

    const auto current_position = camera.position();
    ASSERT_EQ(Vector3::Zero, camera.position());

    camera.update(1.0f, Vector3(0, 1, 0));

    // Check that the camera has moved up.
    const auto new_position = camera.position();
    ASSERT_EQ(0.0f, new_position.x);
    ASSERT_NE(23.0f, new_position.y);
    ASSERT_EQ(0.0f, new_position.z);
}

/// Tests that setting the position actually moves the camera.
TEST(Camera, Position)
{
    Camera camera(Size(100, 100));

    const auto target_position = Vector3(1, 2, 3);
    camera.set_position(target_position);

    const auto new_position = camera.position();
    ASSERT_EQ(target_position, new_position);
}

/// Tests that setting the target of the camera moves the target and camera.
TEST(Camera, Target)
{
    Camera camera(Size(10, 10));

    // Zero rotations to simplify comparisons.
    camera.set_rotation_pitch(0.0f);
    camera.set_rotation_yaw(0.0f);
    camera.set_zoom(1.0f);

    ASSERT_EQ(Vector3(0, 0, -1), camera.position());

    const auto target = Vector3(1, 2, 3);
    camera.set_target(target);

    const auto new_target = camera.target();
    ASSERT_EQ(target, new_target);
    ASSERT_EQ(Vector3(1, 2, 2), camera.position());
}

/// Tests that setting the zoom level of the camera moves the position.
TEST(Camera, Zoom)
{
    Camera camera(Size(10, 10));

    // Zero the rotations on the camera.
    camera.set_rotation_pitch(0.0f);
    camera.set_rotation_yaw(0.0f);

    const auto current_position = camera.position();
    ASSERT_EQ(camera.zoom(), -current_position.z);

    camera.set_zoom(5.0f);
    ASSERT_EQ(5.0f, camera.zoom());

    const auto new_position = camera.position();
    ASSERT_EQ(5.0f, -new_position.z);
}

TEST(Camera, ModeEventRaised)
{
    Camera camera(Size(10, 10));

    std::optional<ICamera::Mode> raised;
    auto token = camera.on_mode_changed += trview::tests::capture(raised);

    camera.set_mode(ICamera::Mode::Axis);

    ASSERT_TRUE(raised);
    ASSERT_EQ(raised.value(), ICamera::Mode::Axis);
}

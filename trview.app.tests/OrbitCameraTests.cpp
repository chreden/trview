#include "gtest/gtest.h"
#include <trview.app/Camera/OrbitCamera.h>

using namespace trview;
using namespace DirectX::SimpleMath;

/// Tests that setting the target of the camera moves the target and camera.
TEST(OrbitCamera, Target)
{
    OrbitCamera camera(Size(10, 10));

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
TEST(OrbitCamera, Zoom)
{
    OrbitCamera camera(Size(10, 10));

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
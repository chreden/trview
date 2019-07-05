#include "gtest/gtest.h"
#include <trview.app/Camera/FreeCamera.h>

using namespace trview;
using namespace DirectX::SimpleMath;

/// Tests that when the camera is in alignment mode it moves relative the world axis.
TEST(FreeCamera, Alignment)
{
    FreeCamera camera(Size(100, 100));

    // Set the camera to be slightly pitched, so that moving up in camera mode
    // would move the camera away from 0,0 on the x/z axis.
    camera.set_rotation_pitch(0.45f);

    // Ensure the camera is in axis mode.
    camera.set_alignment(FreeCamera::Alignment::Axis);

    const auto current_position = camera.position();
    ASSERT_EQ(Vector3::Zero, camera.position());

    camera.move(Vector3(0, 1, 0));

    // Check that the camera has moved up.
    const auto new_position = camera.position();
    ASSERT_EQ(0.0f, new_position.x);
    ASSERT_NE(0.0f, new_position.y);
    ASSERT_EQ(0.0f, new_position.z);
}

/// Tests that setting the position actually moves the camera.
TEST(FreeCamera, Position)
{
    FreeCamera camera(Size(100, 100));

    const auto target_position = Vector3(1, 2, 3);
    camera.set_position(target_position);

    const auto new_position = camera.position();
    ASSERT_EQ(target_position, new_position);
}
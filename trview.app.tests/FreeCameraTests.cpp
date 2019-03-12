#include "CppUnitTest.h"
#include "StringConversions.h"

#include <trview.app/Camera/FreeCamera.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace tests
    {
        TEST_CLASS(FreeCameraTests)
        {
            /// Tests that when the camera is in alignment mode it moves relative the world axis.
            TEST_METHOD(Alignment)
            {
                FreeCamera camera(Size(100, 100));

                // Set the camera to be slightly pitched, so that moving up in camera mode
                // would move the camera away from 0,0 on the x/z axis.
                camera.set_rotation_pitch(0.45f);

                // Ensure the camera is in axis mode.
                camera.set_alignment(FreeCamera::Alignment::Axis);

                const auto current_position = camera.position();
                Assert::AreEqual(Vector3::Zero, camera.position());

                camera.move(Vector3(0, 1, 0));

                // Check that the camera has moved up.
                const auto new_position = camera.position();
                Assert::AreEqual(0.0f, new_position.x);
                Assert::AreNotEqual(0.0f, new_position.y);
                Assert::AreEqual(0.0f, new_position.z);
            }

            /// Tests that setting the position actually moves the camera.
            TEST_METHOD(Position)
            {
                FreeCamera camera(Size(100, 100));

                const auto target_position = Vector3(1, 2, 3);
                camera.set_position(target_position);

                const auto new_position = camera.position();
                Assert::AreEqual(target_position, new_position);
            }
        };
    }
}
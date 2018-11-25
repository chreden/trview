#include "CppUnitTest.h"
#include "StringConversions.h"

#include <trview.app/OrbitCamera.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace tests
    {
        TEST_CLASS(OrbitCameraTests)
        {
            /// Tests that setting the target of the camera moves the target and camera.
            TEST_METHOD(Target)
            {
                OrbitCamera camera(Size(10, 10));

                // Zero rotations to simplify comparisons.
                camera.set_rotation_pitch(0.0f);
                camera.set_rotation_yaw(0.0f);
                camera.set_zoom(1.0f);

                Assert::AreEqual(Vector3(0, 0, -1), camera.position());

                const auto target = Vector3(1, 2, 3);
                camera.set_target(target);

                const auto new_target = camera.target();
                Assert::AreEqual(target, new_target);
                Assert::AreEqual(Vector3(1, 2, 2), camera.position());
            }

            /// Tests that setting the zoom level of the camera moves the position.
            TEST_METHOD(Zoom)
            {
                OrbitCamera camera(Size(10, 10));

                // Zero the rotations on the camera.
                camera.set_rotation_pitch(0.0f);
                camera.set_rotation_yaw(0.0f);

                const auto current_position = camera.position();
                Assert::AreEqual(camera.zoom(), -current_position.z);

                camera.set_zoom(5.0f);
                Assert::AreEqual(5.0f, camera.zoom());

                const auto new_position = camera.position();
                Assert::AreEqual(5.0f, -new_position.z);
            }
        };
    }
}
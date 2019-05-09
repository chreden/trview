#include "CppUnitTest.h"
#include <trview.app/Camera/CameraInput.h>
#include <trview.app.tests/StringConversions.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    namespace tests
    {
        TEST_CLASS(CameraInputTests)
        {
            /// Tests that when the appropriate shortcut key is pressed the event
            /// for entering free mode is raised.
            TEST_METHOD(EnterFreeMode)
            {
                CameraInput subject;

                std::optional<CameraMode> mode;
                auto token = subject.on_mode_change += [&mode](CameraMode new_mode)
                {
                    mode = new_mode;
                };

                subject.key_down(L'F', false);

                Assert::IsTrue(mode.has_value());
                Assert::AreEqual(CameraMode::Free, mode.value());
            }

            /// Tests that when the appropriate shortcut key is pressed the event
            /// for entering axis mode is raised.
            TEST_METHOD(EnterAxisMode)
            {
                CameraInput subject;

                std::optional<CameraMode> mode;
                auto token = subject.on_mode_change += [&mode](CameraMode new_mode)
                {
                    mode = new_mode;
                };

                subject.key_down(L'X', false);

                Assert::IsTrue(mode.has_value());
                Assert::AreEqual(CameraMode::Axis, mode.value());
            }

            /// Tests that when the appropriate shortcut key is pressed the event
            /// for entering orbit mode is raised.
            TEST_METHOD(EnterOrbitMode)
            {
                CameraInput subject;

                std::optional<CameraMode> mode;
                auto token = subject.on_mode_change += [&mode](CameraMode new_mode)
                {
                    mode = new_mode;
                };

                subject.key_down(L'O', false);

                Assert::IsTrue(mode.has_value());
                Assert::AreEqual(CameraMode::Orbit, mode.value());
            }
        };
    }
}
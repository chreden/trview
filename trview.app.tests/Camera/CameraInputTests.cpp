#include "CppUnitTest.h"
#include <trview.app/Camera/CameraInput.h>
#include <trview.app.tests/StringConversions.h>
#include <trview.common/TokenStore.h>
#include <set>

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

            /// Tests that when the mouse is scrolled down the zoom event is raised.
            TEST_METHOD(ZoomOut)
            {
                CameraInput subject;

                std::optional<float> zoom;
                auto token = subject.on_zoom += [&zoom](float new_zoom)
                {
                    zoom = new_zoom;
                };

                subject.mouse_scroll(200);

                Assert::IsTrue(zoom.has_value());
                Assert::IsTrue(zoom.value() < 0.0f);
            }

            /// Tests that when the mouse is scrolled up tzoom event is raised.
            TEST_METHOD(ZoomIn)
            {
                CameraInput subject;

                std::optional<float> zoom;
                auto token = subject.on_zoom += [&zoom](float new_zoom)
                {
                    zoom = new_zoom;
                };

                subject.mouse_scroll(-200);

                Assert::IsTrue(zoom.has_value());
                Assert::IsTrue(zoom.value() > 0.0f);
            }

            /// Tests that right clicking and moving the mouse causes the rotation
            /// event to be raised.
            TEST_METHOD(Rotation)
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

                Assert::IsTrue(rotation.has_value());
                Assert::AreEqual(100.0f, std::get<0>(rotation.value()));
                Assert::AreEqual(200.0f, std::get<1>(rotation.value()));
            }

            /// Tests that giving the appropriate input to the camera input results in the
            /// correct movement vector.
            TEST_METHOD(Movement)
            {
                using namespace DirectX::SimpleMath;

                CameraInput subject;

                Assert::AreEqual(Vector3::Zero, subject.movement());

                subject.key_down(L'W', false);
                Assert::AreEqual(Vector3(0, 0, 1), subject.movement());

                subject.key_down(L'A', false);
                Assert::AreEqual(Vector3(-1, 0, 1), subject.movement());

                subject.key_up(L'A');
                Assert::AreEqual(Vector3(0, 0, 1), subject.movement());

                subject.key_down(L'D', false);
                Assert::AreEqual(Vector3(1, 0, 1), subject.movement());

                subject.key_up(L'W');
                Assert::AreEqual(Vector3(1, 0, 0), subject.movement());

                subject.key_down(L'S', false);
                Assert::AreEqual(Vector3(1, 0, -1), subject.movement());

                subject.key_up(L'S');
                Assert::AreEqual(Vector3(1, 0, 0), subject.movement());

                subject.key_up(L'D');
                Assert::AreEqual(Vector3::Zero, subject.movement());
            }

            /// Tests that if the control key is pressed nothing is done.
            TEST_METHOD(ControlBlocks)
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
                Assert::IsTrue(modes.empty());

                subject.key_down('W', true);
                subject.key_down('A', true);
                Assert::AreEqual(Vector3::Zero, subject.movement());

                subject.key_down('S', true);
                subject.key_down('D', true);
                Assert::AreEqual(Vector3::Zero, subject.movement());
            }
        };
    }
}
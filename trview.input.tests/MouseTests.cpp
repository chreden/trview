#include "CppUnitTest.h"

#include <trview.input/Mouse.h>
#include "WindowHelper.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Microsoft 
{
    namespace VisualStudio 
    {
        namespace CppUnitTestFramework
        {
            std::wstring ToString(const trview::input::Mouse::Button& button)
            {
                using namespace trview::input;
                switch (button)
                {
                case Mouse::Button::Left:
                    return L"Left";
                case Mouse::Button::Right:
                    return L"Right";
                }
                return L"Unknown";
            }
        }
    }
}

namespace trview
{
    namespace input
    {
        TEST_CLASS(MouseTests)
        {
        public:
            TEST_METHOD(MouseDown)
            {
                int times_called = 0;
                Mouse::Button button_received = Mouse::Button::Left;

                HWND window = create_test_window();
                Mouse mouse(window);

                mouse.mouse_down += 
                    [&times_called, &button_received](auto button)
                {
                    ++times_called;
                    button_received = button;
                };

                RAWINPUT input;
                memset(&input, 0, sizeof(input));
                input.header.dwType = RIM_TYPEMOUSE;
                input.header.dwSize = sizeof(RAWINPUT);
                input.data.mouse.usButtonFlags |= RI_MOUSE_RIGHT_BUTTON_DOWN;
                mouse.process_input(input);

                Assert::AreEqual(1, times_called);
                Assert::AreEqual(Mouse::Button::Right, button_received);
            }

            TEST_METHOD(MouseUp)
            {
                int times_called = 0;
                Mouse::Button button_received = Mouse::Button::Left;

                HWND window = create_test_window();
                Mouse mouse(window);

                mouse.mouse_up +=
                    [&times_called, &button_received](auto button)
                {
                    ++times_called;
                    button_received = button;
                };

                RAWINPUT input;
                memset(&input, 0, sizeof(input));
                input.header.dwType = RIM_TYPEMOUSE;
                input.header.dwSize = sizeof(RAWINPUT);
                input.data.mouse.usButtonFlags |= RI_MOUSE_RIGHT_BUTTON_UP;
                mouse.process_input(input);

                Assert::AreEqual(1, times_called);
                Assert::AreEqual(Mouse::Button::Right, button_received);
            }

            TEST_METHOD(MouseWheel)
            {
                Assert::Fail();
            }

            TEST_METHOD(MouseMove)
            {
                Assert::Fail();
            }

            TEST_METHOD(MousePosition)
            {
                Assert::Fail();
            }
        };
    }
}

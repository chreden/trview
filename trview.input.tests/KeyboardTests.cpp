#include "CppUnitTest.h"

#include <trview.input/Keyboard.h>
#include <trview.tests.common/Window.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace trview::tests;

namespace trview
{
    namespace input
    {
        TEST_CLASS(KeyboardTests)
        {
        public:
            /// Tests that when a WM_KEYDOWN message is posted to the window that the keyboard is watching
            /// the key down event is raised.
            TEST_METHOD(KeyDown)
            {
                int times_called = 0;
                uint16_t key_received = 0;

                HWND window = create_test_window(L"TRViewInputTests");
                Keyboard keyboard(window);
                auto token = keyboard.on_key_down +=
                    [&times_called, &key_received](uint16_t key, bool control) 
                {
                    ++times_called; 
                    key_received = key; 
                };

                SendMessage(window, WM_KEYDOWN, VK_SPACE, 0);

                Assert::AreEqual(1, times_called);
                Assert::AreEqual(static_cast<int>(VK_SPACE), static_cast<int>(key_received));
            }

            /// Tests that when a WM_KEYUP message is posted to the window that the keyboard is watching
            /// the key up event is raised.
            TEST_METHOD(KeyUp)
            {
                int times_called = 0;
                uint16_t key_received = 0;

                HWND window = create_test_window(L"TRViewInputTests");
                Keyboard keyboard(window);
                auto token = keyboard.on_key_up +=
                    [&times_called, &key_received](uint16_t key, bool control)
                {
                    ++times_called;
                    key_received = key;
                };

                SendMessage(window, WM_KEYUP, VK_SPACE, 0);

                Assert::AreEqual(1, times_called);
                Assert::AreEqual(static_cast<int>(VK_SPACE), static_cast<int>(key_received));
            }

            /// Tests that when a WM_CHAR message is posted to the window that the keyboard is watching
            /// the char event is raised.
            TEST_METHOD(Char)
            {
                int times_called = 0;
                uint16_t char_received = 0;

                HWND window = create_test_window(L"TRViewInputTests");
                Keyboard keyboard(window);
                auto token = keyboard.on_char +=
                    [&times_called, &char_received](uint16_t key)
                {
                    ++times_called;
                    char_received = key;
                };

                SendMessage(window, WM_CHAR, 'A', 0);

                Assert::AreEqual(1, times_called);
                Assert::AreEqual(static_cast<int>('A'), static_cast<int>(char_received));
            }
        };
    }
}

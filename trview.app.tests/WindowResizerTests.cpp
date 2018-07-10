#include "CppUnitTest.h"

#include <trview.app/WindowResizer.h>
#include <trview.tests.common/Window.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    namespace tests
    {
        TEST_CLASS(WindowResizerTests)
        {
            // Tests that entering and exiting resizing raises the resize event.
            TEST_METHOD(SizeMove)
            {
                HWND window = create_test_window(L"TRViewWindowResizerTests");
                WindowResizer resizer(window);

                uint32_t times_called = 0;
                resizer.on_resize += [&]() { ++times_called; };

                resizer.process_message(window, WM_ENTERSIZEMOVE, 0, 0);
                resizer.process_message(window, WM_EXITSIZEMOVE, 0, 0);

                Assert::AreEqual(1u, times_called);
            }

            // Test that sending a maximize message triggers the event.
            TEST_METHOD(Maximise)
            {
                HWND window = create_test_window(L"TRViewWindowResizerTests");
                WindowResizer resizer(window);

                uint32_t times_called = 0;
                resizer.on_resize += [&]() { ++times_called; };

                resizer.process_message(window, WM_SIZE, SIZE_MAXIMIZED, 0);

                Assert::AreEqual(1u, times_called);
            }

            // Test that sending a minimise message triggers the event.
            TEST_METHOD(Minimise)
            {
                HWND window = create_test_window(L"TRViewWindowResizerTests");
                WindowResizer resizer(window);

                uint32_t times_called = 0;
                resizer.on_resize += [&]() { ++times_called; };

                resizer.process_message(window, WM_SIZE, SIZE_RESTORED, 0);

                Assert::AreEqual(1u, times_called);
            }

            // Test that size messages during resize don't trigger the event.
            TEST_METHOD(Resizing)
            {
                HWND window = create_test_window(L"TRViewWindowResizerTests");
                WindowResizer resizer(window);

                uint32_t times_called = 0;
                resizer.on_resize += [&]() { ++times_called; };

                resizer.process_message(window, WM_ENTERSIZEMOVE, 0, 0);
                Assert::AreEqual(0u, times_called);

                resizer.process_message(window, WM_SIZE, 0, 0);
                Assert::AreEqual(0u, times_called);

                resizer.process_message(window, WM_EXITSIZEMOVE, 0, 0);
                Assert::AreEqual(1u, times_called);
            }
        };
    }
}

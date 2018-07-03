#include "CppUnitTest.h"

#include <trview.app/WindowResizer.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace trview
{
    namespace
    {
        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        HWND create_test_window()
        {
            HINSTANCE hInstance = GetModuleHandle(nullptr);

            WNDCLASSEXW wcex;
            memset(&wcex, 0, sizeof(wcex));

            wcex.cbSize = sizeof(WNDCLASSEX);

            wcex.style = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc = WndProc;
            wcex.hInstance = hInstance;
            wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wcex.lpszClassName = L"TRViewWindowResizerTests";

            RegisterClassExW(&wcex);

            HWND window = CreateWindowW(L"TRViewWindowResizerTests", L"TRViewWindowResizerTests", WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
            ShowWindow(window, SW_HIDE);
            UpdateWindow(window);
            return window;
        }
    }

    namespace tests
    {
        TEST_CLASS(WindowResizerTests)
        {
            // Tests that entering and exiting resizing raises the resize event.
            TEST_METHOD(SizeMove)
            {
                HWND window = create_test_window();
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
                HWND window = create_test_window();
                WindowResizer resizer(window);

                uint32_t times_called = 0;
                resizer.on_resize += [&]() { ++times_called; };

                resizer.process_message(window, WM_SIZE, SIZE_MAXIMIZED, 0);

                Assert::AreEqual(1u, times_called);
            }

            // Test that sending a minimise message triggers the event.
            TEST_METHOD(Minimise)
            {
                HWND window = create_test_window();
                WindowResizer resizer(window);

                uint32_t times_called = 0;
                resizer.on_resize += [&]() { ++times_called; };

                resizer.process_message(window, WM_SIZE, SIZE_RESTORED, 0);

                Assert::AreEqual(1u, times_called);
            }

            // Test that size messages during resize don't trigger the event.
            TEST_METHOD(Resizing)
            {
                HWND window = create_test_window();
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

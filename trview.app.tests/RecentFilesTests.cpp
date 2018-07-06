#include "CppUnitTest.h"

#include <trview.app/RecentFiles.h>

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
            wcex.lpszClassName = L"TRViewRecentFilesTests";

            RegisterClassExW(&wcex);

            HWND window = CreateWindowW(L"TRViewRecentFilesTests", L"TRViewRecentFilesTests", WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
            ShowWindow(window, SW_HIDE);
            UpdateWindow(window);
            return window;
        }
    }

    namespace tests
    {
        TEST_CLASS(RecentFilesTests)
        {
            /// Tests that opening the first file triggers the event.
            TEST_METHOD(OpenFile)
            {
                HWND window = create_test_window();
                RecentFiles recent_files(window);

                const std::list<std::wstring> files{ L"test.tr2", L"test2.tr2" };
                recent_files.set_recent_files(files);

                uint32_t times_called{ 0 };
                std::wstring raised_file;
                recent_files.on_file_open += [&](const auto& file)
                {
                    ++times_called;
                    raised_file = file;
                };

                recent_files.process_message(window, WM_COMMAND, MAKEWPARAM(5000, 0), 0);
                Assert::AreEqual(1u, times_called);
                Assert::AreEqual(std::wstring(L"test2.tr2"), files.back());
            }
        };
    }
}

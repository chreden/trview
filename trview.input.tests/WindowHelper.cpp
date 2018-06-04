#include "WindowHelper.h"

namespace trview
{
    namespace input
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
            wcex.lpszClassName = L"TRViewInputTests";

            RegisterClassExW(&wcex);

            HWND window = CreateWindowW(L"TRViewInputTests", L"TRViewInputTests", WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
            ShowWindow(window, SW_HIDE);
            UpdateWindow(window);
            return window;
        }
    }
}
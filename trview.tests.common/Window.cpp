#include "Window.h"

namespace trview
{
    namespace tests
    {
        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        Window create_test_window(const std::wstring& name)
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
            wcex.lpszClassName = name.c_str();

            RegisterClassExW(&wcex);

            HWND window = CreateWindowW(name.c_str(), name.c_str(), WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
            ShowWindow(window, SW_HIDE);
            UpdateWindow(window);
            return window;
        }
    }
}
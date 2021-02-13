#include "Application.h"
#include "Resources/resource.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <commdlg.h>
#include <trview.common/Strings.h>

namespace trview
{
    namespace
    {
        const std::wstring window_class{ L"TRVIEW" };
        const std::wstring window_title{ L"trview" };

        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        ATOM register_class(HINSTANCE hInstance)
        {
            WNDCLASSEXW wcex;

            wcex.cbSize = sizeof(WNDCLASSEX);

            wcex.style = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc = WndProc;
            wcex.cbClsExtra = 0;
            wcex.cbWndExtra = 0;
            wcex.hInstance = hInstance;
            wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRVIEW));
            wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_TRVIEW);
            wcex.lpszClassName = window_class.c_str();
            wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

            return RegisterClassExW(&wcex);
        }

        HWND create_window(HINSTANCE hInstance, int nCmdShow)
        {
            HWND window = CreateWindowW(window_class.c_str(), window_title.c_str(), WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

            if (!window)
            {
                return nullptr;
            }

            ShowWindow(window, nCmdShow);
            UpdateWindow(window);

            return window;
        }
    }

    Application::Application(HINSTANCE hInstance, const std::wstring& command_line, int command_show)
        : _instance(hInstance)
    {
        register_class(_instance);

        HWND window = create_window(_instance, command_show);
        if (!window)
        {
            // Convert to throw?
            return;
        }

        _viewer = std::make_unique<Viewer>(window);

        // Open the level passed in on the command line, if there is one.
        int number_of_arguments = 0;
        const LPWSTR* const arguments = CommandLineToArgvW(command_line.c_str(), &number_of_arguments);
        if (number_of_arguments > 1)
        {
            _viewer->open(trview::to_utf8(arguments[1]));
        }
    }

    int Application::run()
    {
        HACCEL hAccelTable = LoadAccelerators(_instance, MAKEINTRESOURCE(IDC_TRVIEW));

        MSG msg;
        memset(&msg, 0, sizeof(msg));

        while (msg.message != WM_QUIT)
        {
            while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    break;
                }

                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }

            _viewer->render();
            Sleep(1);
        }

        return (int)msg.wParam;
    }
}
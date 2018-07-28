#include "ItemsWindow.h"
#include <Windows.h>
#include <trview/resource.h>
#include <trview.graphics/DeviceWindow.h>
#include <SimpleMath.h>

using namespace trview::graphics;

namespace trview
{
    namespace
    {
        LRESULT CALLBACK items_window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        HWND init_items_instance(HWND parent, HINSTANCE hInstance, int nCmdShow)
        {
            HWND items_window = CreateWindowW(L"trview.items", L"trview", WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, 0, 400, 300, parent, nullptr, hInstance, nullptr);

            ShowWindow(items_window, nCmdShow);
            UpdateWindow(items_window);

            return items_window;
        }

        ATOM register_items_class(HINSTANCE hInstance)
        {
            WNDCLASSEXW wcex;
            wcex.cbSize = sizeof(WNDCLASSEX);
            wcex.style = CS_HREDRAW | CS_VREDRAW;
            wcex.lpfnWndProc = items_window_procedure;
            wcex.cbClsExtra = 0;
            wcex.cbWndExtra = 0;
            wcex.hInstance = hInstance;
            wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRVIEW));
            wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
            wcex.lpszMenuName = nullptr;
            wcex.lpszClassName = L"trview.items";
            wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

            return RegisterClassExW(&wcex);
        }

        HWND create_items_window(HWND parent)
        {
            HINSTANCE hInstance = GetModuleHandle(nullptr);
            register_items_class(hInstance);
            return init_items_instance(parent, hInstance, SW_NORMAL);
        }
    }

    ItemsWindow::ItemsWindow(const Device& device, HWND parent)
        : MessageHandler(create_items_window(parent)), _window_resizer(window()), _device_window(device.create_for_window(window()))
    {
        _window_resizer.on_resize += [=]() { _device_window->resize(); };
    }

    void ItemsWindow::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
    }

    void ItemsWindow::render(bool vsync)
    {
        _device_window->begin();
        _device_window->clear(DirectX::SimpleMath::Color(0.0f, 0.2f, 0.4f, 1.0f));
        _device_window->present(vsync);
    }
}

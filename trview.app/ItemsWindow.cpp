#include "ItemsWindow.h"
#include <Windows.h>
#include <random>
#include <trview/resource.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.ui/Control.h>
#include <trview.ui/Window.h>
#include <trview.ui/Button.h>
#include <trview.ui.render/Renderer.h>
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

    ItemsWindow::ItemsWindow(const Device& device, const IShaderStorage& shader_storage, const FontFactory& font_factory, HWND parent)
        : MessageHandler(create_items_window(parent)), _window_resizer(window()), _device_window(device.create_for_window(window())),
        _ui_renderer(std::make_unique<ui::render::Renderer>(device.device(), shader_storage, font_factory, Window(window()).size())),
        _mouse(window())
    {
        _token_store.add(_window_resizer.on_resize += [=]()
        {
            _device_window->resize();
            _ui->set_size(window().size());
        });
        generate_ui();

        _token_store.add(_mouse.mouse_up += [&](auto) { _ui->mouse_up(client_cursor_position(window())); });
        _token_store.add(_mouse.mouse_move += [&](auto, auto) { _ui->mouse_move(client_cursor_position(window())); });
        _token_store.add(_mouse.mouse_down += [&](input::Mouse::Button) { _ui->mouse_down(client_cursor_position(window())); });
    }

    void ItemsWindow::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
    }

    void ItemsWindow::render(const Device& device, bool vsync)
    {
        _device_window->begin();
        _device_window->clear(DirectX::SimpleMath::Color(0.0f, 0.2f, 0.4f, 1.0f));
        _ui_renderer->render(device.context());
        _device_window->present(vsync);
    }

    void ItemsWindow::generate_ui()
    {
        _ui = std::make_unique<ui::Window>(Point(), window().size(), Colour(1.0f, 0.5f, 0.5f, 0.5f));
        auto button = std::make_unique<ui::Button>(Point(5, 5), Size(100, 30), L"Test button");
        _token_store.add(button->on_click += [&]()
        {
            std::random_device device;
            std::default_random_engine engine(device());
            std::uniform_real_distribution<float> random;
            _ui->set_background_colour(Colour(1.0f, random(engine), random(engine), random(engine)));
        });
        _ui->add_child(std::move(button));
        _ui_renderer->load(_ui.get());
    }
}

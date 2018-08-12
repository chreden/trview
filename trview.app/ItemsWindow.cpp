#include "ItemsWindow.h"
#include <Windows.h>
#include <trview/resource.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.ui.render/Renderer.h>
#include <SimpleMath.h>

using namespace trview::graphics;

namespace trview
{
    namespace
    {
        LRESULT CALLBACK items_window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            if (message == WM_GETMINMAXINFO)
            {
                MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lParam);
                info->ptMinTrackSize.x = 400;
                info->ptMinTrackSize.y = 200;
                return 0;
            }

            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        HWND init_items_instance(HWND parent, HINSTANCE hInstance, int nCmdShow)
        {
            HWND items_window = CreateWindowW(L"trview.items", L"trview", WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, 0, 400, 310, parent, nullptr, hInstance, nullptr);

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
        _ui_renderer(std::make_unique<ui::render::Renderer>(device.device(), shader_storage, font_factory, window().size())),
        _mouse(window())
    {
        _token_store.add(_window_resizer.on_resize += [=]()
        {
            _device_window->resize();
            _ui->set_size(window().size());
            _items_list->set_size(window().size());
        });
        generate_ui();

        _token_store.add(_mouse.mouse_up += [&](auto) { _ui->mouse_up(client_cursor_position(window())); });
        _token_store.add(_mouse.mouse_move += [&](auto, auto) { _ui->mouse_move(client_cursor_position(window())); });
        _token_store.add(_mouse.mouse_down += [&](input::Mouse::Button) { _ui->mouse_down(client_cursor_position(window())); });
        _token_store.add(_mouse.mouse_wheel += [&](int16_t delta) { _ui->mouse_scroll(client_cursor_position(window()), delta); });
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
        using namespace ui;

        _ui = std::make_unique<ui::Window>(Point(), window().size(), Colour(0.0f, 0.5f, 0.5f, 0.5f));
        auto items_list = std::make_unique<Listbox>(Point(), window().size());
        items_list->set_columns(
            { 
                { Listbox::Column::Type::Number, L"#" }, 
                { Listbox::Column::Type::Number, L"Room"},
                { Listbox::Column::Type::Number, L"Type"} }
            );
        _items_list = items_list.get();
        _ui->add_child(std::move(items_list));
        _ui_renderer->load(_ui.get());
    }

    void ItemsWindow::set_items(const std::vector<Item>& items)
    {
        _items = items;

        using namespace ui;
        std::vector<Listbox::Item> list_items;
        for (const auto& item : _items)
        {
            list_items.push_back({
                {
                    { L"#", std::to_wstring(item.number()) },
                    { L"Room", std::to_wstring(item.room()) },
                    { L"Type", item.type() }
                } });
        }
        _items_list->set_items(list_items);
    }
}

#include "ItemsWindow.h"
#include <Windows.h>
#include <trview/resource.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.ui.render/Renderer.h>
#include <SimpleMath.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/GroupBox.h>

using namespace trview::graphics;

namespace trview
{
    namespace
    {
        const DWORD window_style = WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

        LRESULT CALLBACK items_window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            if (message == WM_GETMINMAXINFO)
            {
                RECT rect{ 0, 0, 400, 200 };
                AdjustWindowRect(&rect, window_style, FALSE);

                MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lParam);
                info->ptMinTrackSize.x = rect.right - rect.left;
                info->ptMaxTrackSize.x = rect.right - rect.left;
                info->ptMinTrackSize.y = 200;
                return 0;
            }

            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        HWND init_items_instance(HWND parent, HINSTANCE hInstance, int nCmdShow)
        {
            HWND items_window = CreateWindowW(L"trview.items", L"Items", window_style,
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
            update_layout();
            _ui_renderer->set_host_size(window().size());
        });
        generate_ui();

        _token_store.add(_mouse.mouse_up += [&](auto) { _ui->mouse_up(client_cursor_position(window())); });
        _token_store.add(_mouse.mouse_move += [&](auto, auto) { _ui->mouse_move(client_cursor_position(window())); });
        _token_store.add(_mouse.mouse_down += [&](input::Mouse::Button) { _ui->mouse_down(client_cursor_position(window())); });
        _token_store.add(_mouse.mouse_wheel += [&](int16_t delta) { _ui->mouse_scroll(client_cursor_position(window()), delta); });
    }

    void ItemsWindow::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_CLOSE)
        {
            on_window_closed();
        }
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

        _ui = std::make_unique<ui::StackPanel>(Point(), window().size(), Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(0, 3), StackPanel::Direction::Horizontal, SizeMode::Manual);

        auto left_panel = std::make_unique<ui::StackPanel>(Point(), Size(200, window().size().height), Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(0,3), StackPanel::Direction::Vertical, SizeMode::Manual);

        // Control modes:.
        auto controls = std::make_unique<StackPanel>(Point(), Size(window().size().width, 20), Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(2,2), StackPanel::Direction::Horizontal, SizeMode::Manual);
        auto track_room = std::make_unique<Checkbox>(Point(), Size(16, 16), L"Track Room");
        _token_store.add(track_room->on_state_changed += [this](bool value)
        {
            _track_room = value;
            if (_track_room)
            {
                set_current_room(_current_room);
            }
            else
            {
                set_items(_all_items);
            }
        });

        controls->add_child(std::move(track_room));
        _controls = controls.get();
        left_panel->add_child(std::move(controls));

        auto items_list = std::make_unique<Listbox>(Point(), window().size() - Size(0, _controls->size().height));
        items_list->set_columns(
            { 
                { Listbox::Column::Type::Number, L"#", 30 }, 
                { Listbox::Column::Type::Number, L"Room", 30 },
                { Listbox::Column::Type::Number, L"ID", 30 },
                { Listbox::Column::Type::String, L"Type", 100 } }
            );
        _token_store.add(items_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            on_item_selected(_all_items[index]);
        });

        _items_list = items_list.get();
        left_panel->add_child(std::move(items_list));

        _left_panel = left_panel.get();
        _ui->add_child(std::move(left_panel));

        _ui->add_child(create_details_panel());

        _ui_renderer->load(_ui.get());
    }

    void ItemsWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
        populate_items(items);
    }

    void ItemsWindow::populate_items(const std::vector<Item>& items)
    {
        using namespace ui;
        std::vector<Listbox::Item> list_items;
        for (const auto& item : items)
        {
            list_items.push_back({
                {
                    { L"#", std::to_wstring(item.number()) },
                    { L"ID", std::to_wstring(item.type_id()) },
                    { L"Room", std::to_wstring(item.room()) },
                    { L"Type", item.type() }
                } });
        }
        _items_list->set_items(list_items);
    }

    void ItemsWindow::set_current_room(uint32_t room)
    {
        _current_room = room;
        if (_track_room)
        {
            std::vector<Item> filtered_items;
            for (const auto& item : _all_items)
            {
                if (item.room() == room)
                {
                    filtered_items.push_back(item);
                }
            }
            populate_items(filtered_items);
        }
    }

    void ItemsWindow::update_layout()
    {
        _ui->set_size(window().size());
        _left_panel->set_size(Size(200, window().size().height));
        _right_panel->set_size(Size(200, window().size().height));
        _items_list->set_size(Size(200, window().size().height - _controls->size().height));
    }

    std::unique_ptr<ui::StackPanel> ItemsWindow::create_details_panel()
    {
        using namespace ui;

        const float height = window().size().height;

        auto right_panel = std::make_unique<StackPanel>(Point(), Size(200, height), Colour(1.0f, 0.5f, 0.3f, 0.5f), Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);

        auto group_box = std::make_unique<GroupBox>(Point(), Size(200, height), Colour(1.0f, 0.5f, 0.5f, 0.5f), Colour(1.0f, 0.0f, 0.0f, 0.0f), L"Item Details");
        right_panel->add_child(std::move(group_box));

        // Store the right panel for later use.
        _right_panel = right_panel.get();
        return right_panel;
    }
}

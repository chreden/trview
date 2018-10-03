#include "TriggersWindow.h"
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.common/Colour.h>
#include <trview/resource.h>
#include <trview.ui.render/Renderer.h>

namespace trview
{
    namespace
    {
        const DWORD window_style = WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

        /// Colours commonly used in this class.
        namespace Colours
        {
            const Colour Divider{ 1.0f, 0.0f, 0.0f, 0.0f };
            const Colour LeftPanel{ 1.0f, 0.4f, 0.4f, 0.4f };
            const Colour ItemDetails{ 1.0f, 0.35f, 0.35f, 0.35f };
            const Colour Triggers{ 1.0f, 0.3f, 0.3f, 0.3f };
            const Colour DetailsBorder{ 0.0f, 0.0f, 0.0f, 0.0f };
        }

        LRESULT CALLBACK items_window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        HWND init_items_instance(HWND parent, HINSTANCE hInstance, int nCmdShow)
        {
            RECT rect{ 0, 0, 400, 400 };
            AdjustWindowRect(&rect, window_style, FALSE);

            HWND items_window = CreateWindowW(L"trview.triggers", L"Triggers", window_style,
                CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, parent, nullptr, hInstance, nullptr);

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
            wcex.lpszClassName = L"trview.triggers";
            wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

            return RegisterClassExW(&wcex);
        }

        HWND create_items_window(HWND parent)
        {
            HINSTANCE hInstance = GetModuleHandle(nullptr);
            register_items_class(hInstance);
            return init_items_instance(parent, hInstance, SW_NORMAL);
        }

        ui::Listbox::Item create_listbox_item(const Trigger& item)
        {
            return { {} };
            //return { {{ L"#", std::to_wstring(item.number()) },
            //         { L"ID", std::to_wstring(item.type_id()) },
            //         { L"Room", std::to_wstring(item.room()) },
            //         { L"Type", item.type() }} };
        }
    }

    using namespace graphics;

    TriggersWindow::TriggersWindow(const Device& device, const IShaderStorage& shader_storage, const FontFactory& font_factory, HWND parent)
        : MessageHandler(create_items_window(parent)), _window_resizer(window()), _device_window(device.create_for_window(window())),
        _ui_renderer(std::make_unique<ui::render::Renderer>(device.device(), shader_storage, font_factory, window().size())),
        _mouse(window()), _keyboard(window())
    {
        _token_store.add(_window_resizer.on_resize += [=]()
        {
            _device_window->resize();
            update_layout();
            _ui_renderer->set_host_size(window().size());
        });
        generate_ui();

        _token_store.add(_mouse.mouse_up += [&](auto) { _ui->process_mouse_up(client_cursor_position(window())); });
        _token_store.add(_mouse.mouse_move += [&](auto, auto) { _ui->process_mouse_move(client_cursor_position(window())); });
        _token_store.add(_mouse.mouse_down += [&](input::Mouse::Button) { _ui->process_mouse_down(client_cursor_position(window())); });
        _token_store.add(_mouse.mouse_wheel += [&](int16_t delta) { _ui->mouse_scroll(client_cursor_position(window()), delta); });
        _token_store.add(_keyboard.on_key_down += [&](auto key) { _ui->process_key_down(key); });
    }

    void TriggersWindow::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (message == WM_CLOSE)
        {
            on_window_closed();
        }
        else if (message == WM_GETMINMAXINFO)
        {
            RECT rect{ 0, 0, _ui->size().width, 400 };
            AdjustWindowRect(&rect, window_style, FALSE);

            MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMinTrackSize.x = rect.right - rect.left;
            info->ptMaxTrackSize.x = rect.right - rect.left;
            info->ptMinTrackSize.y = rect.bottom - rect.top;
        }
    }

    void TriggersWindow::render(const Device& device, bool vsync)
    {
        _device_window->begin();
        _device_window->clear(DirectX::SimpleMath::Color(0.0f, 0.2f, 0.4f, 1.0f));
        _ui_renderer->render(device.context());
        _device_window->present(vsync);
    }

    void TriggersWindow::generate_ui()
    {
        using namespace ui;
        _ui = std::make_unique<StackPanel>(Point(), window().size(), Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(0, 0), StackPanel::Direction::Horizontal, SizeMode::Manual);
        // _ui->add_child(create_items_panel());
        // _ui->add_child(create_divider());
        // _ui->add_child(create_details_panel());
        _ui_renderer->load(_ui.get());
    }

    void TriggersWindow::set_triggers(const std::vector<Trigger>& triggers)
    {
        _all_triggers = triggers;
        populate_triggers(triggers);
    }

    void TriggersWindow::clear_selected_trigger()
    {
    }

    void TriggersWindow::populate_triggers(const std::vector<Trigger>& triggers)
    {
        using namespace ui;
        std::vector<Listbox::Item> list_items;
        std::transform(triggers.begin(), triggers.end(), std::back_inserter(list_items), create_listbox_item);
        _triggers_list->set_items(list_items);
    }

    void TriggersWindow::set_current_room(uint32_t room)
    {
        if (_track_room && (!_filter_applied || _current_room != room))
        {
            _filter_applied = true;

            std::vector<Trigger> filtered_triggers;
            for (const auto& trigger : _all_triggers)
            {
                if (trigger.room() == room)
                {
                    filtered_triggers.push_back(trigger);
                }
            }
            populate_triggers(filtered_triggers);
        }

        _current_room = room;
    }

    void TriggersWindow::set_selected_trigger(const Trigger& trigger)
    {
        _selected_trigger = trigger;
        if (_sync_trigger)
        {
            const auto& list_item = create_listbox_item(trigger);
            if (_triggers_list->set_selected_item(list_item))
            {
                // load_item_details(item);
            }
            else
            {
                _selected_trigger.reset();
            }
        }
    }

    void TriggersWindow::update_layout()
    {
        _ui->set_size(window().size());
        const auto new_height = window().size().height;
        // _left_panel->set_size(Size(_left_panel->size().width, new_height));
        // _divider->set_size(Size(_divider->size().width, new_height));
        // _right_panel->set_size(Size(_right_panel->size().width, new_height));
        // _items_list->set_size(Size(_items_list->size().width, _left_panel->size().height - _items_list->position().y));
    }
}

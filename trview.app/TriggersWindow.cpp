#include "TriggersWindow.h"
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.common/Colour.h>
#include <trview/resource.h>
#include <trview.ui.render/Renderer.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Button.h>

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
            return { {{ L"#", std::to_wstring(item.number()) },
                     { L"Room", std::to_wstring(item.room()) },
                     { L"Type", trigger_type_name(item.type()) }} };
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
        _ui->add_child(create_triggers_panel());
        _ui->add_child(create_divider());
        // _ui->add_child(create_details_panel());
        _ui_renderer->load(_ui.get());
    }

    std::unique_ptr<ui::StackPanel> TriggersWindow::create_triggers_panel()
    {
        using namespace ui;
        auto left_panel = std::make_unique<ui::StackPanel>(Point(), Size(200, window().size().height), Colours::LeftPanel, Size(0, 3), StackPanel::Direction::Vertical, SizeMode::Manual);

        // Control modes:.
        auto controls = std::make_unique<StackPanel>(Point(), Size(200, 20), Colours::LeftPanel, Size(2, 2), StackPanel::Direction::Horizontal, SizeMode::Manual);
        auto track_room = std::make_unique<Checkbox>(Point(), Size(16, 16), Colours::LeftPanel, L"Track Room");
        _token_store.add(track_room->on_state_changed += [this](bool value)
        {
            set_track_room(value);
        });

        _track_room_checkbox = controls->add_child(std::move(track_room));

        // Spacing between checkboxes.
        controls->add_child(std::make_unique<ui::Window>(Point(), Size(5, 20), Colours::LeftPanel));

        auto sync_trigger = std::make_unique<Checkbox>(Point(), Size(16, 16), Colours::LeftPanel, L"Sync Trigger");
        sync_trigger->set_state(_sync_trigger);
        _token_store.add(sync_trigger->on_state_changed += [this](bool value) { set_sync_trigger(value); });
        controls->add_child(std::move(sync_trigger));

        // Space out the button
        controls->add_child(std::make_unique<ui::Window>(Point(), Size(5, 20), Colours::LeftPanel));

        auto expander = std::make_unique<Button>(Point(), Size(16, 16), L"<<");
        _token_store.add(expander->on_click += [this]()
        {
            toggle_expand();
        });
        _expander = controls->add_child(std::move(expander));

        _controls = left_panel->add_child(std::move(controls));

        auto triggers_list = std::make_unique<Listbox>(Point(), Size(200, window().size().height - _controls->size().height), Colours::LeftPanel);
        triggers_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::Number, L"Room", 30 },
                { Listbox::Column::Type::String, L"Type", 130 }
            }
        );
        _token_store.add(triggers_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            // load_item_details(_all_items[index]);
            if (_sync_trigger)
            {
                on_trigger_selected(_all_triggers[index]);
            }
        });

        _triggers_list = triggers_list.get();
        left_panel->add_child(std::move(triggers_list));

        // Fix items list size now that it has been added to the panel.
        _triggers_list->set_size(Size(200, left_panel->size().height - _triggers_list->position().y));

        _left_panel = left_panel.get();
        return left_panel;
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
        _left_panel->set_size(Size(_left_panel->size().width, new_height));
        _divider->set_size(Size(_divider->size().width, new_height));
        // _right_panel->set_size(Size(_right_panel->size().width, new_height));
        _triggers_list->set_size(Size(_triggers_list->size().width, _left_panel->size().height - _triggers_list->position().y));
    }

    std::unique_ptr<ui::Control> TriggersWindow::create_divider()
    {
        auto divider = std::make_unique<ui::Window>(Point(), Size(1, window().size().height), Colours::Divider);
        _divider = divider.get();
        return divider;
    }

    void TriggersWindow::set_sync_trigger(bool value)
    {
        if (_sync_trigger != value)
        {
            _sync_trigger = value;
            if (_selected_trigger.has_value())
            {
                set_selected_trigger(_selected_trigger.value());
            }
        }
    }

    void TriggersWindow::toggle_expand()
    {
        _expanded = !_expanded;
        _expander->set_text(_expanded ? L"<<" : L">>");
        _ui->set_size(Size(_expanded ? 400 : 200, _ui->size().height));

        // Force resize the window.
        RECT rect{ 0, 0, _ui->size().width, _ui->size().height };
        AdjustWindowRect(&rect, window_style, FALSE);
        SetWindowPos(window(), 0, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);
    }

    void TriggersWindow::set_track_room(bool value)
    {
        if (_track_room != value)
        {
            _track_room = value;
            if (_track_room)
            {
                set_current_room(_current_room);
            }
            else
            {
                set_triggers(_all_triggers);
                _filter_applied = false;
            }
        }

        if (_track_room_checkbox->state() != _track_room)
        {
            _track_room_checkbox->set_state(_track_room);
        }
    }
}

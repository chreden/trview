#include "ItemsWindow.h"
#include <sstream>
#include <bitset>
#include <Windows.h>
#include <trview/resource.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.ui.render/Renderer.h>
#include <SimpleMath.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/GroupBox.h>
#include <trview.ui/Button.h>

using namespace trview::graphics;

namespace trview
{
    namespace
    {
        const DWORD window_style = WS_OVERLAPPEDWINDOW & ~(WS_MINIMIZEBOX | WS_MAXIMIZEBOX);

        /// Colours commonly used in this class.
        namespace Colours
        {
            const Colour Divider { 1.0f, 0.0f, 0.0f, 0.0f };
            const Colour LeftPanel { 1.0f, 0.4f, 0.4f, 0.4f };
            const Colour ItemDetails { 1.0f, 0.35f, 0.35f, 0.35f };
            const Colour Triggers { 1.0f, 0.3f, 0.3f, 0.3f };
            const Colour DetailsBorder { 0.0f, 0.0f, 0.0f, 0.0f };
        }

        LRESULT CALLBACK items_window_procedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        HWND init_items_instance(HWND parent, HINSTANCE hInstance, int nCmdShow)
        {
            RECT rect{ 0, 0, 400, 400 };
            AdjustWindowRect(&rect, window_style, FALSE);

            HWND items_window = CreateWindowW(L"trview.items", L"Items", window_style,
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

    void ItemsWindow::process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam)
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
        _ui = std::make_unique<StackPanel>(Point(), window().size(), Colour(1.0f, 0.5f, 0.5f, 0.5f), Size(0, 0), StackPanel::Direction::Horizontal, SizeMode::Manual);
        _ui->add_child(create_items_panel());
        _ui->add_child(create_divider());
        _ui->add_child(create_details_panel());
        _ui_renderer->load(_ui.get());
    }

    void ItemsWindow::set_items(const std::vector<Item>& items)
    {
        _all_items = items;
        populate_items(items);
    }

    void ItemsWindow::set_triggers(const std::vector<Trigger>& triggers)
    {
        _all_triggers = triggers;
        _trigger_list->set_items({});
    }

    void ItemsWindow::clear_selected_item()
    {
        _stats_list->set_items({});
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
        if (_track_room && (!_filter_applied || _current_room != room))
        {
            _filter_applied = true;

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

        _current_room = room;
    }

    void ItemsWindow::update_layout()
    {
        _ui->set_size(window().size());
        const auto new_height = window().size().height;
        _left_panel->set_size(Size(_left_panel->size().width, new_height));
        _divider->set_size(Size(_divider->size().width, new_height));
        _right_panel->set_size(Size(_right_panel->size().width, new_height));
        _items_list->set_size(Size(_items_list->size().width, _left_panel->size().height - _items_list->position().y));
    }

    std::unique_ptr<ui::StackPanel> ItemsWindow::create_items_panel()
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

        auto expander = std::make_unique<Button>(Point(), Size(16, 16), L"<<");
        _token_store.add(expander->on_click += [this]()
        {
            _expanded = !_expanded;
            _expander->set_text(_expanded ? L"<<" : L">>");
        });
        _expander = controls->add_child(std::move(expander));

        _controls = left_panel->add_child(std::move(controls));

        auto items_list = std::make_unique<Listbox>(Point(), Size(200, window().size().height - _controls->size().height), Colours::LeftPanel);
        items_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 30 },
                { Listbox::Column::Type::Number, L"Room", 30 },
                { Listbox::Column::Type::Number, L"ID", 30 },
                { Listbox::Column::Type::String, L"Type", 100 } 
            }
        );
        _token_store.add(items_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            load_item_details(_all_items[index]);
            on_item_selected(_all_items[index]);
        });

        _items_list = items_list.get();
        left_panel->add_child(std::move(items_list));

        // Fix items list size now that it has been added to the panel.
        _items_list->set_size(Size(200, left_panel->size().height - _items_list->position().y));

        _left_panel = left_panel.get();
        return left_panel;
    }

    std::unique_ptr<ui::Control> ItemsWindow::create_divider()
    {
        auto divider = std::make_unique<ui::Window>(Point(), Size(1, window().size().height), Colours::Divider);
        _divider = divider.get();
        return divider;
    }

    std::unique_ptr<ui::StackPanel> ItemsWindow::create_details_panel()
    {
        using namespace ui;

        const float height = window().size().height;

        auto right_panel = std::make_unique<StackPanel>(Point(), Size(200, height), Colours::ItemDetails, Size(), StackPanel::Direction::Vertical, SizeMode::Manual);
        auto group_box = std::make_unique<GroupBox>(Point(), Size(200, 190), Colours::ItemDetails, Colours::DetailsBorder, L"Item Details");

        // Add some information about the selected item.
        auto stats_list = std::make_unique<Listbox>(Point(10,21), Size(180, 160), Colours::ItemDetails);
        stats_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"Name", 60 },
                { Listbox::Column::Type::Number, L"Value", 120 },
            }
        );
        stats_list->set_show_headers(false);
        stats_list->set_show_scrollbar(false);
        stats_list->set_show_highlight(false);

        _stats_list = group_box->add_child(std::move(stats_list));

        right_panel->add_child(std::make_unique<ui::Window>(Point(), Size(200, 8), Colours::ItemDetails));
        right_panel->add_child(std::move(group_box));

        // Spacer element.
        right_panel->add_child(std::make_unique<ui::Window>(Point(), Size(200, 5), Colours::Triggers));

        // Add the trigger details group box.
        auto trigger_group_box = std::make_unique<GroupBox>(Point(), Size(200, 200), Colours::Triggers, Colours::DetailsBorder, L"Triggered By");

        auto trigger_list = std::make_unique<Listbox>(Point(10, 21), Size(190, 160), Colours::Triggers);
        trigger_list->set_columns(
            {
                { Listbox::Column::Type::Number, L"#", 25 },
                { Listbox::Column::Type::Number, L"Room", 35 },
                { Listbox::Column::Type::String, L"Type", 120 },
            }
        );
        trigger_list->set_show_headers(true);
        trigger_list->set_show_scrollbar(true);
        trigger_list->set_show_highlight(false);

        _token_store.add(trigger_list->on_item_selected += [&](const auto& item)
        {
            auto index = std::stoi(item.value(L"#"));
            set_track_room(false);
            on_trigger_selected(_all_triggers[index]);
        });

        _trigger_list = trigger_group_box->add_child(std::move(trigger_list));
        right_panel->add_child(std::move(trigger_group_box));

        // Store the right panel for later use.
        _right_panel = right_panel.get();
        return right_panel;
    }

    void ItemsWindow::load_item_details(const Item& item)
    {
        auto format_bool = [](bool value)
        {
            std::wstringstream stream;
            stream << std::boolalpha << value;
            return stream.str();
        };

        auto format_binary = [](uint16_t value)
        {
            std::wstringstream stream;
            stream << std::bitset<5>(value);
            return stream.str();
        };

        auto make_item = [](const auto& name, const auto& value)
        {
            return Listbox::Item { { { L"Name", name }, { L"Value", value } } };
        };

        using namespace ui;
        std::vector<Listbox::Item> stats;
        stats.push_back(make_item(L"Type", item.type()));
        stats.push_back(make_item(L"#", std::to_wstring(item.number())));
        stats.push_back(make_item(L"Type ID", std::to_wstring(item.type_id())));
        stats.push_back(make_item(L"Room", std::to_wstring(item.room())));
        stats.push_back(make_item(L"Clear Body", format_bool(item.clear_body_flag())));
        stats.push_back(make_item(L"Invisible", format_bool(item.invisible_flag())));
        stats.push_back(make_item(L"Flags", format_binary(item.activation_flags())));
        stats.push_back(make_item(L"OCB", std::to_wstring(item.ocb())));
        _stats_list->set_items(stats);

        std::vector<Listbox::Item> triggers;
        for (auto& trigger : item.triggers())
        {
            triggers.push_back(
                {
                    {
                        { L"#", std::to_wstring(trigger.number()) },
                        { L"Room", std::to_wstring(trigger.room()) },
                        { L"Type", trigger_type_name(trigger.type()) },
                    }
                });
        }
        _trigger_list->set_items(triggers);
    }

    void ItemsWindow::set_track_room(bool value)
    {
        _track_room = value;
        if (_track_room)
        {
            set_current_room(_current_room);
        }
        else
        {
            set_items(_all_items);
            _filter_applied = false;
        }

        if (_track_room_checkbox->state() != _track_room)
        {
            _track_room_checkbox->set_state(_track_room);
        }
    }
}

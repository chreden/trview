#include "Application.h"

#include <shellapi.h>
#include <Shlwapi.h>
#include <commdlg.h>

#include <trlevel/trlevel.h>

#include <trview.app/Graphics/TextureStorage.h>
#include <trview.common/Strings.h>
#include <trview.graphics/ShaderStorage.h>

#include "Resources/resource.h"
#include "Resources/ResourceHelper.h"
#include "Resources/DefaultShaders.h"
#include "Resources/DefaultFonts.h"
#include "Resources/DefaultTextures.h"
#include "Elements/TypeNameLookup.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const std::wstring window_class{ L"TRVIEW" };
        const std::wstring window_title{ L"trview" };

        INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
        {
            UNREFERENCED_PARAMETER(lParam);
            switch (message)
            {
                case WM_INITDIALOG:
                    return (INT_PTR)TRUE;
                case WM_COMMAND:
                    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
                    {
                        EndDialog(hDlg, LOWORD(wParam));
                        return (INT_PTR)TRUE;
                    }
                    break;
            }
            return (INT_PTR)FALSE;
        }

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
            register_class(hInstance);

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
        : MessageHandler(create_window(hInstance, command_show)), _instance(hInstance),
        _file_dropper(window()), _level_switcher(window()), _recent_files(window()), _update_checker(window()),
        _shortcuts(window()), _view_menu(window())
    {
        _update_checker.check_for_updates();
        _settings = load_user_settings();

        _token_store += _file_dropper.on_file_dropped += [&](const auto& file) { open(file); };

        _token_store += _level_switcher.on_switch_level += [=](const auto& file) { open(file); };
        _token_store += on_file_loaded += [&](const auto& file) { _level_switcher.open_file(file); };

        _recent_files.set_recent_files(_settings.recent_files);
        _token_store += _recent_files.on_file_open += [=](const auto& file) { open(file); };
        _token_store += on_recent_files_changed += [&](const auto& files) { _recent_files.set_recent_files(files); };

        Resource type_list = get_resource_memory(IDR_TYPE_NAMES, L"TEXT");
        _type_name_lookup = std::make_unique<TypeNameLookup>(std::string(type_list.data, type_list.data + type_list.size));

        _shader_storage = std::make_unique<graphics::ShaderStorage>();
        load_default_shaders(_device, *_shader_storage.get());
        load_default_fonts(_device, _font_factory);

        _texture_storage = std::make_unique<TextureStorage>(_device);
        load_default_textures(_device, *_texture_storage.get());

        _route = std::make_unique<Route>(_device, *_shader_storage);

        setup_shortcuts();
        setup_view_menu();
        setup_items_windows();
        setup_triggers_windows();
        setup_rooms_windows();
        setup_route_window();
        setup_viewer(command_line);
    }

    Application::~Application()
    {
        save_user_settings(_settings);
    }

    void Application::open(const std::string& filename)
    {
        std::unique_ptr<trlevel::ILevel> new_level;
        try
        {
            new_level = trlevel::load_level(filename);
        }
        catch (...)
        {
            MessageBox(window(), L"Failed to load level", L"Error", MB_OK);
            return;
        }

        on_file_loaded(filename);
        _settings.add_recent_file(filename);
        on_recent_files_changed(_settings.recent_files);
        save_user_settings(_settings);
        _viewer->set_settings(_settings);

        _level = std::make_unique<Level>(_device, *_shader_storage.get(), std::move(new_level), *_type_name_lookup);
        _level->set_filename(filename);
        _token_store += _level->on_room_selected += [&](uint16_t room) { select_room(room); };

        _viewer->open(_level.get());

        _items_windows->set_items(_level->items());
        _items_windows->set_triggers(_level->triggers());
        _triggers_windows->set_items(_level->items());
        _triggers_windows->set_triggers(_level->triggers());
        _rooms_windows->set_items(_level->items());
        _rooms_windows->set_triggers(_level->triggers());
        _rooms_windows->set_rooms(_level->rooms());
        _route_window->set_items(_level->items());
        _route_window->set_triggers(_level->triggers());
        _route_window->set_rooms(_level->rooms());

        _route->clear();
        _route_window->set_route(_route.get());
        _viewer->set_route(_route.get());
    }

    void Application::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        switch (message)
        {
            case WM_COMMAND:
            {
                int wmId = LOWORD(wParam);

                // Parse the menu selections:
                switch (wmId)
                {
                    case IDM_ABOUT:
                        DialogBox(GetModuleHandle(nullptr), MAKEINTRESOURCE(IDD_ABOUTBOX), window(), About);
                        break;
                    case ID_FILE_OPEN:
                    case ID_ACCEL_FILE_OPEN:
                    {
                        wchar_t cd[MAX_PATH];
                        GetCurrentDirectoryW(MAX_PATH, cd);

                        OPENFILENAME ofn;
                        memset(&ofn, 0, sizeof(ofn));

                        wchar_t path[MAX_PATH];
                        memset(&path, 0, sizeof(path));

                        ofn.lStructSize = sizeof(ofn);
                        ofn.lpstrFile = path;
                        ofn.nMaxFile = MAX_PATH;
                        ofn.lpstrTitle = L"Open level";
                        ofn.lpstrFilter = L"All Tomb Raider Files\0*.tr*;*.phd\0";
                        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

                        if (GetOpenFileName(&ofn))
                        {
                            SetCurrentDirectory(cd);
                            open(trview::to_utf8(ofn.lpstrFile));
                        }
                        break;
                    }
                    case ID_HELP_GITHUB:
                    {
                        ShellExecute(0, 0, L"https://github.com/chreden/trview", 0, 0, SW_SHOW);
                        break;
                    }
                    case ID_HELP_DISCORD:
                    {
                        ShellExecute(0, 0, L"https://discord.gg/Zy7kYge", 0, 0, SW_SHOW);
                        break;
                    }
                    case IDM_EXIT:
                        DestroyWindow(window());
                        break;
                }
                break;
            }
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                break;
            }
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

            render();
            Sleep(1);
        }

        return (int)msg.wParam;
    }

    void Application::setup_view_menu()
    {
        _token_store += _view_menu.on_show_minimap += [&](bool show) { _viewer->set_show_minimap(show); };
        _token_store += _view_menu.on_show_tooltip += [&](bool show) { _viewer->set_show_tooltip(show); };
        _token_store += _view_menu.on_show_ui += [&](bool show) { _viewer->set_show_ui(show); };
        _token_store += _view_menu.on_show_compass += [&](bool show) { _viewer->set_show_compass(show); };
        _token_store += _view_menu.on_show_selection += [&](bool show) { _viewer->set_show_selection(show); };
        _token_store += _view_menu.on_show_route += [&](bool show) { _viewer->set_show_route(show); };
        _token_store += _view_menu.on_show_tools += [&](bool show) { _viewer->set_show_tools(show); };
        _token_store += _view_menu.on_colour_change += [&](Colour colour)
        {
            _settings.background_colour = static_cast<uint32_t>(colour);
            _viewer->set_settings(_settings);
        };
        _token_store += _view_menu.on_unhide_all += [&]()
        {
            for (const auto& item : _level->items()) { if (!item.visible()) { set_item_visibility(item, true); } }
            for (const auto& trigger : _level->triggers()) { if (!trigger->visible()) { set_trigger_visibility(trigger, true); } }
        };
    }

    void Application::setup_viewer(const std::wstring& command_line)
    {
        _viewer = std::make_unique<Viewer>(window(), _device, *_shader_storage.get(), _font_factory, *_texture_storage, _shortcuts, _route.get());
        _token_store += _viewer->on_item_visibility += [this](const auto& item, bool value) { set_item_visibility(item, value); };
        _token_store += _viewer->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _viewer->on_room_selected += [this](uint32_t room) { select_room(room); };
        _token_store += _viewer->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
        _token_store += _viewer->on_trigger_visibility += [this](const auto& trigger, bool value) { set_trigger_visibility(trigger, value); };
        _token_store += _viewer->on_waypoint_added += [this](const auto& position, auto room, auto type, auto index) { add_waypoint(position, room, type, index); };
        _token_store += _viewer->on_waypoint_selected += [this](auto index) { select_waypoint(index); };
        _token_store += _viewer->on_waypoint_removed += [this](auto index) { remove_waypoint(index); };
        _token_store += _viewer->on_waypoint_select_previous += [this]() { select_previous_waypoint(); };
        _token_store += _viewer->on_waypoint_select_next += [this]() { select_next_waypoint(); };
        _viewer->set_settings(_settings);

        // Open the level passed in on the command line, if there is one.
        int number_of_arguments = 0;
        const LPWSTR* const arguments = CommandLineToArgvW(command_line.c_str(), &number_of_arguments);
        if (number_of_arguments > 1)
        {
            open(trview::to_utf8(arguments[1]));
        }
    }

    void Application::setup_items_windows()
    {
        _items_windows = std::make_unique<ItemsWindowManager>(_device, *_shader_storage, _font_factory, window(), _shortcuts);
        if (_settings.items_startup)
        {
            _items_windows->create_window();
        }

        _token_store += _items_windows->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _items_windows->on_item_visibility += [this](const auto& item, bool state) { set_item_visibility(item, state); };
        _token_store += _items_windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
        _token_store += _items_windows->on_add_to_route += [this](const auto& item)
        {
            uint32_t new_index = _route->insert(item.position(), item.room(), Waypoint::Type::Entity, item.number());
            _route_window->set_route(_route.get());
            select_waypoint(new_index);
        };
    }

    void Application::setup_triggers_windows()
    {
        _triggers_windows = std::make_unique<TriggersWindowManager>(_device, *_shader_storage, _font_factory, window(), _shortcuts);
        if (_settings.triggers_startup)
        {
            _triggers_windows->create_window();
        }
        _token_store += _triggers_windows->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _triggers_windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
        _token_store += _triggers_windows->on_trigger_visibility += [this](const auto& trigger, bool state) { set_trigger_visibility(trigger, state); };
        _token_store += _triggers_windows->on_add_to_route += [this](const auto& trigger)
        {
            add_waypoint(trigger->position(), trigger->room(), Waypoint::Type::Trigger, trigger->number());
        };
    }

    void Application::setup_rooms_windows()
    {
        _rooms_windows = std::make_unique<RoomsWindowManager>(_device, *_shader_storage, _font_factory, window(), _shortcuts);
        if (_settings.rooms_startup)
        {
            _rooms_windows->create_window();
        }

        _token_store += _rooms_windows->on_room_selected += [this](const auto& room) { select_room(room); };
        _token_store += _rooms_windows->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _rooms_windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
    }

    void Application::setup_route_window()
    {
        _route_window = std::make_unique<RouteWindowManager>(_device, *_shader_storage, _font_factory, window(), _shortcuts);
        _token_store += _route_window->on_waypoint_selected += [&](auto index) { select_waypoint(index); };
        _token_store += _route_window->on_item_selected += [&](const auto& item) { select_item(item); };
        _token_store += _route_window->on_trigger_selected += [&](const auto& trigger) { select_trigger(trigger); };
        _token_store += _route_window->on_route_import += [&](const std::string& path)
        {
            auto route = import_route(_device, *_shader_storage, path);
            if (route)
            {
                _route = std::move(route);
                _route_window->set_route(_route.get());
                _viewer->set_route(_route.get());
            }
        };
        _token_store += _route_window->on_route_export += [&](const std::string& path) { export_route(*_route, path); };
        _token_store += _route_window->on_waypoint_deleted += [&](auto index) { remove_waypoint(index); };
        _token_store += _route_window->on_colour_changed += [&](const Colour& colour)
        {
            _route->set_colour(colour);
            _viewer->set_route(_route.get());
        };
    }

    void Application::setup_shortcuts()
    {
        auto add_shortcut = [&](bool control, uint16_t key, std::function<void()> fn)
        {
            _token_store += _shortcuts.add_shortcut(control, key) += [&, fn]()
            {
                if (!_viewer->ui_input_active()) { fn(); }
            };
        };

        add_shortcut(false, VK_LEFT, [&]() { select_previous_waypoint(); });
        add_shortcut(false, VK_RIGHT, [&]() { select_next_waypoint(); });
        add_shortcut(false, VK_DELETE, [&]() { remove_waypoint(_route->selected_waypoint()); });
    }

    void Application::add_waypoint(const Vector3& position, uint32_t room, Waypoint::Type type, uint32_t index)
    {
        uint32_t new_index = _route->insert(position, room, type, index);
        _route_window->set_route(_route.get());
        select_waypoint(new_index);
    }

    void Application::remove_waypoint(uint32_t index)
    {
        _route->remove(index);
        _route_window->set_route(_route.get());
        _viewer->set_route(_route.get());

        if (_route->waypoints() > 0)
        {
            select_waypoint(_route->selected_waypoint());
        }
    }

    void Application::select_item(const Item& item)
    {
        if (!_level || item.number() >= _level->items().size())
        {
            return;
        }

        select_room(item.room());
        _level->set_selected_item(item.number());
        _viewer->select_item(item);
        _items_windows->set_selected_item(item);
        _rooms_windows->set_selected_item(item);
    }

    void Application::select_room(uint32_t room)
    {
        if (!_level || room >= _level->number_of_rooms())
        {
            return;
        }

        _level->set_selected_room(static_cast<uint16_t>(room));
        _viewer->select_room(room);
        _items_windows->set_room(room);
        _rooms_windows->set_room(room);
        _triggers_windows->set_room(room);
    }

    void Application::select_trigger(const Trigger* const trigger)
    {
        if (!_level)
        {
            return;
        }

        select_room(trigger->room());
        _level->set_selected_trigger(trigger->number());
        _viewer->select_trigger(trigger);
        _triggers_windows->set_selected_trigger(trigger);
        _rooms_windows->set_selected_trigger(trigger);
    }

    void Application::select_waypoint(uint32_t index)
    {
        const auto& waypoint = _route->waypoint(index);
        select_room(_route->waypoint(index).room());
        _route->select_waypoint(index);
        _viewer->select_waypoint(waypoint);
        _route_window->select_waypoint(index);
    }

    void Application::select_next_waypoint()
    {
        if (_route->selected_waypoint() + 1 < _route->waypoints())
        {
            select_waypoint(_route->selected_waypoint() + 1);
        }
    }

    void Application::select_previous_waypoint()
    {
        if (_route->selected_waypoint() > 0)
        {
            select_waypoint(_route->selected_waypoint() - 1);
        }
    }

    void Application::set_item_visibility(const Item& item, bool visible)
    {
        if (!_level)
        {
            return;
        }

        _level->set_item_visibility(item.number(), visible);
        _items_windows->set_item_visible(item, visible);
    }

    void Application::set_trigger_visibility(Trigger* const trigger, bool visible)
    {
        if (!_level)
        {
            return;
        }

        _level->set_trigger_visibility(trigger->number(), visible);
        _triggers_windows->set_trigger_visible(trigger, visible);
    }

    void Application::render()
    {
        // If minimised, don't render like crazy. Sleep so we don't hammer the CPU either.
        if (window_is_minimised(window()))
        {
            Sleep(1);
            return;
        }

        _viewer->render();
        _items_windows->render(_device, _settings.vsync);
        _triggers_windows->render(_device, _settings.vsync);
        _rooms_windows->render(_device, _settings.vsync);
        _route_window->render(_device, _settings.vsync);
    }
}
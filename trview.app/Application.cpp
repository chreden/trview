#include "Application.h"

#include "Resources/resource.h"
#include "Resources/DefaultShaders.h"
#include "Resources/DefaultFonts.h"
#include "Resources/DefaultTextures.h"

#include <external/boost/di.hpp>

#include <trlevel/di.h>
#include <trview.graphics/di.h>
#include <trview.ui/di.h>
#include <trview.ui.render/di.h>
#include <trview.input/di.h>
#include <trview.app/Elements/di.h>
#include <trview.app/Geometry/di.h>
#include <trview.app/Graphics/di.h>
#include <trview.app/Menus/di.h>
#include <trview.app/Routing/di.h>
#include <trview.app/Settings/di.h>
#include <trview.app/Tools/di.h>
#include <trview.app/UI/di.h>
#include <trview.app/Windows/di.h>
#include <trview.common/Files.h>
#include <trview.common/windows/Clipboard.h>
#include <trview.common/Windows/Dialogs.h>
#include <trview.app/Settings/IStartupOptions.h>

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

        Window create_window(HINSTANCE hInstance, int nCmdShow)
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

    IApplication::~IApplication()
    {
    }

    Application::Application(const Window& application_window,
        std::unique_ptr<IUpdateChecker> update_checker,
        std::unique_ptr<ISettingsLoader> settings_loader,
        std::unique_ptr<IFileDropper> file_dropper,
        std::unique_ptr<trlevel::ILevelLoader> level_loader,
        std::unique_ptr<ILevelSwitcher> level_switcher,
        std::unique_ptr<IRecentFiles> recent_files,
        std::unique_ptr<IViewer> viewer,
        const IRoute::Source& route_source,
        std::shared_ptr<IShortcuts> shortcuts,
        std::unique_ptr<IItemsWindowManager> items_window_manager,
        std::unique_ptr<ITriggersWindowManager> triggers_window_manager,
        std::unique_ptr<IRouteWindowManager> route_window_manager,
        std::unique_ptr<IRoomsWindowManager> rooms_window_manager,
        const ILevel::Source& level_source,
        std::shared_ptr<IStartupOptions> startup_options,
        std::shared_ptr<IDialogs> dialogs,
        std::shared_ptr<IFiles> files)
        : MessageHandler(application_window), _instance(GetModuleHandle(nullptr)),
        _file_dropper(std::move(file_dropper)), _level_switcher(std::move(level_switcher)), _recent_files(std::move(recent_files)), _update_checker(std::move(update_checker)),
        _view_menu(window()), _settings_loader(std::move(settings_loader)), _level_loader(std::move(level_loader)), _viewer(std::move(viewer)), _route_source(route_source),
        _route(route_source()), _shortcuts(shortcuts), _items_windows(std::move(items_window_manager)),
        _triggers_windows(std::move(triggers_window_manager)), _route_window(std::move(route_window_manager)), _rooms_windows(std::move(rooms_window_manager)), _level_source(level_source),
        _dialogs(dialogs), _files(files), _timer(default_time_source())
    {
        _update_checker->check_for_updates();
        _settings = _settings_loader->load_user_settings();

        _token_store += _file_dropper->on_file_dropped += [&](const auto& file) { open(file); };
        _token_store += _level_switcher->on_switch_level += [=](const auto& file) { open(file); };

        _recent_files->set_recent_files(_settings.recent_files);
        _token_store += _recent_files->on_file_open += [=](const auto& file) { open(file); };

        setup_shortcuts();
        setup_view_menu();
        setup_items_windows();
        setup_triggers_windows();
        setup_rooms_windows();
        setup_route_window();
        setup_viewer(*startup_options);

        register_lua();
        lua_init(&lua_registry);
    }

    Application::~Application()
    {
        _settings_loader->save_user_settings(_settings);
    }

    void Application::open(const std::string& filename)
    {
        if (!should_discard_changes())
        {
            return;
        }

        std::unique_ptr<trlevel::ILevel> new_level;
        try
        {
            new_level = _level_loader->load_level(filename);
        }
        catch (...)
        {
            _dialogs->message_box(window(), L"Failed to load level", L"Error", IDialogs::Buttons::OK);
            return;
        }

        _level_switcher->open_file(filename);
        _settings.add_recent_file(filename);
        _recent_files->set_recent_files(_settings.recent_files);
        _settings_loader->save_user_settings(_settings);
        _viewer->set_settings(_settings);

        _level = _level_source(std::move(new_level));
        _level->set_filename(filename);

        _items_windows->set_items(_level->items());
        _items_windows->set_triggers(_level->triggers());
        _triggers_windows->set_items(_level->items());
        _triggers_windows->set_triggers(_level->triggers());
        _rooms_windows->set_level_version(_level->version());
        _rooms_windows->set_items(_level->items());
        _rooms_windows->set_triggers(_level->triggers());
        _rooms_windows->set_rooms(_level->rooms());
        _route_window->set_items(_level->items());
        _route_window->set_triggers(_level->triggers());
        _route_window->set_rooms(_level->rooms());
        _route->clear();
        _route->set_unsaved(false);
        _route_window->set_route(_route.get());

        _viewer->open(_level.get());
        _viewer->set_route(_route);
    }

    std::optional<int> Application::process_message(UINT message, WPARAM wParam, LPARAM)
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
                        const auto filename = _dialogs->open_file(L"Open level", { { L"All Tomb Raider Files", { L"*.tr*", L"*.phd" } } }, OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST);
                        if (filename.has_value())
                        {
                            open(filename.value().filename);
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
                    {
                        if (should_discard_changes())
                        {
                            on_closing();
                            DestroyWindow(window());
                        }
                        break;
                    }
                }
                break;
            }
            case WM_CLOSE:
            {
                if (should_discard_changes())
                {
                    on_closing();
                    DestroyWindow(window());
                }
                return 0;
            }
            case WM_DESTROY:
            {
                PostQuitMessage(0);
                break;
            }
        }
        return {};
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
            for (const auto& trigger : _level->triggers()) { set_trigger_visibility(trigger, true); }
        };
    }

    void Application::setup_viewer(const IStartupOptions& startup_options)
    {
        _token_store += _viewer->on_item_visibility += [this](const auto& item, bool value) { set_item_visibility(item, value); };
        _token_store += _viewer->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _viewer->on_room_selected += [this](uint32_t room) { select_room(room); };
        _token_store += _viewer->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
        _token_store += _viewer->on_trigger_visibility += [this](const auto& trigger, bool value) { set_trigger_visibility(trigger, value); };
        _token_store += _viewer->on_waypoint_added += [this](const auto& position, const auto& normal, auto room, auto type, auto index) { add_waypoint(position, normal, room, type, index); };
        _token_store += _viewer->on_waypoint_selected += [this](auto index) { select_waypoint(index); };
        _token_store += _viewer->on_waypoint_removed += [this](auto index) { remove_waypoint(index); };
        _token_store += _viewer->on_settings += [this](auto&& settings)
        {
            _settings = settings;
            _viewer->set_settings(_settings);
            _route_window->set_randomizer_enabled(settings.randomizer_tools);
        };
        _viewer->set_settings(_settings);

        auto filename = startup_options.filename();
        if (!filename.empty())
        {
            open(filename);
        }
    }

    void Application::setup_items_windows()
    {
        if (_settings.items_startup)
        {
            _items_windows->create_window();
        }

        _token_store += _items_windows->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _items_windows->on_item_visibility += [this](const auto& item, bool state) { set_item_visibility(item, state); };
        _token_store += _items_windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
        _token_store += _items_windows->on_add_to_route += [this](const auto& item)
        {
            add_waypoint(item.position(), Vector3::Down, item.room(), IWaypoint::Type::Entity, item.number());
        };
    }

    void Application::setup_triggers_windows()
    {
        if (_settings.triggers_startup)
        {
            _triggers_windows->create_window();
        }
        _token_store += _triggers_windows->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _triggers_windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
        _token_store += _triggers_windows->on_trigger_visibility += [this](const auto& trigger, bool state) { set_trigger_visibility(trigger, state); };
        _token_store += _triggers_windows->on_add_to_route += [this](const auto& trigger)
        {
            if (auto trigger_ptr = trigger.lock())
            {
                add_waypoint(trigger_ptr->position(), Vector3::Down, trigger_ptr->room(), IWaypoint::Type::Trigger, trigger_ptr->number());
            }
        };
    }

    void Application::setup_rooms_windows()
    {
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
        _token_store += _route_window->on_waypoint_selected += [&](auto index) { select_waypoint(index); };
        _token_store += _route_window->on_item_selected += [&](const auto& item) { select_item(item); };
        _token_store += _route_window->on_trigger_selected += [&](const auto& trigger) { select_trigger(trigger); };
        _token_store += _route_window->on_route_import += [&](const std::string& path, bool is_rando)
        {
            auto route = import_route(_route_source, _files, path, _level.get(), is_rando);
            if (route)
            {
                _route = route;
                _route_window->set_route(_route.get());
                _viewer->set_route(_route);
            }
        };
        _token_store += _route_window->on_route_export += [&](const std::string& path, bool is_rando)
        {
            export_route(*_route, _files, path, _level ? _level->filename() : "", is_rando); 
            _route->set_unsaved(false);
        };
        _token_store += _route_window->on_waypoint_deleted += [&](auto index) { remove_waypoint(index); };
        _token_store += _route_window->on_colour_changed += [&](const Colour& colour)
        {
            _route->set_colour(colour);
            _viewer->set_route(_route);
        };
        _route_window->set_randomizer_enabled(_settings.randomizer_tools);
    }

    void Application::setup_shortcuts()
    {
        auto add_shortcut = [&](bool control, uint16_t key, std::function<void()> fn)
        {
            _token_store += _shortcuts->add_shortcut(control, key) += [&, fn]()
            {
                if (!_viewer->ui_input_active()) { fn(); }
            };
        };

        add_shortcut(false, VK_LEFT, [&]() { select_previous_waypoint(); });
        add_shortcut(false, VK_RIGHT, [&]() { select_next_waypoint(); });
        add_shortcut(false, VK_DELETE, [&]() { remove_waypoint(_route->selected_waypoint()); });
    }

    void Application::add_waypoint(const Vector3& position, const Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t index)
    {
        uint32_t new_index = _route->insert(position, normal, room, type, index);
        _route_window->set_route(_route.get());
        select_waypoint(new_index);
    }

    void Application::remove_waypoint(uint32_t index)
    {
        _route->remove(index);
        _route_window->set_route(_route.get());
        _viewer->set_route(_route);

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

    void Application::select_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        if (!_level)
        {
            return;
        }

        auto trigger_ptr = trigger.lock();
        select_room(trigger_ptr->room());
        _level->set_selected_trigger(trigger_ptr->number());
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

    void Application::set_trigger_visibility(const std::weak_ptr<ITrigger>& trigger, bool visible)
    {
        if (!_level)
        {
            return;
        }

        if (const auto trigger_ptr = trigger.lock())
        {
            if (trigger_ptr->visible() != visible)
            {
                _level->set_trigger_visibility(trigger_ptr->number(), visible);
                _triggers_windows->set_trigger_visible(trigger, visible);
            }
        }
    }

    void Application::render()
    {
        // If minimised, don't render like crazy. Sleep so we don't hammer the CPU either.
        if (window_is_minimised(window()))
        {
            Sleep(1);
            return;
        }

        _timer.update();
        const auto elapsed = _timer.elapsed();
        _items_windows->update(elapsed);
        _triggers_windows->update(elapsed);
        _rooms_windows->update(elapsed);
        _route_window->update(elapsed);

        _viewer->render();
        _items_windows->render(_settings.vsync);
        _triggers_windows->render(_settings.vsync);
        _rooms_windows->render(_settings.vsync);
        _route_window->render(_settings.vsync);
    }

    bool Application::should_discard_changes()
    {
        if (_route->is_unsaved())
        {
            return _dialogs->message_box(window(), L"Route has unsaved changes. Do you want to continue and lose changes?", L"Unsaved Route Changes", IDialogs::Buttons::Yes_No);
        }
        return true;
    }

    std::unique_ptr<IApplication> create_application(HINSTANCE instance, const std::wstring& command_line, int command_show)
    {
        using namespace boost;
        using namespace graphics;

        const auto injector = di::make_injector(
            graphics::register_module(),
            input::register_module(),
            trlevel::register_module(),
            ui::register_module(),
            ui::render::register_module(),
            register_app_elements_module(),
            register_app_geometry_module(),
            register_app_graphics_module(),
            register_app_menus_module(),
            register_app_routing_module(),
            register_app_settings_module(),
            register_app_tools_module(),
            register_app_ui_module(),
            register_app_windows_module(),
            di::bind<Window>.to(create_window(instance, command_show)),
            di::bind<IClipboard>.to<Clipboard>(),
            di::bind<IShortcuts>.to<Shortcuts>(),
            di::bind<IShortcuts::Source>.to(
                [](const auto& injector) -> IShortcuts::Source
                {
                    return [](auto&& window)
                    {
                        return std::make_shared<Shortcuts>(window);
                    };
                }
            ),
            di::bind<IApplication>.to<Application>(),
            di::bind<IDialogs>.to<Dialogs>(),
            di::bind<IFiles>.to<Files>(),
            di::bind<IStartupOptions::CommandLine>.to(command_line)
        );

        load_default_shaders(
            injector.create<std::shared_ptr<graphics::IDevice>>(),
            injector.create<std::shared_ptr<IShaderStorage>>());
        load_default_fonts(
            injector.create<std::shared_ptr<graphics::IDevice>>(),
            injector.create<std::shared_ptr<IFontFactory>>());
        load_default_textures(
            injector.create<std::shared_ptr<graphics::IDevice>>(),
            injector.create<std::shared_ptr<ITextureStorage>>());

        return injector.create<std::unique_ptr<IApplication>>();
    }
}
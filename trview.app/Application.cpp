#include "Application.h"
#include "Resources/resource.h"
#include <shellapi.h>
#include <Shlwapi.h>
#include <commdlg.h>
#include <trview.common/Strings.h>
#include <trlevel/trlevel.h>

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
        : MessageHandler(create_window(hInstance, command_show)), _instance(hInstance), _viewer(window()),
        _level_switcher(window()), _recent_files(window())
    {
        _settings = load_user_settings();

        _token_store += _level_switcher.on_switch_level += [=](const auto& file) { open(file); };
        _token_store += on_file_loaded += [&](const auto& file) { _level_switcher.open_file(file); };

        _recent_files.set_recent_files(_settings.recent_files);
        _token_store += _recent_files.on_file_open += [=](const auto& file) { open(file); };
        _token_store += on_recent_files_changed += [&](const auto& files) { _recent_files.set_recent_files(files); };

        // Open the level passed in on the command line, if there is one.
        int number_of_arguments = 0;
        const LPWSTR* const arguments = CommandLineToArgvW(command_line.c_str(), &number_of_arguments);
        if (number_of_arguments > 1)
        {
            _viewer.open(trview::to_utf8(arguments[1]));
        }
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
        _viewer.set_settings(_settings);

        _viewer.open(filename);

        /*
        _level = std::make_unique<Level>(_device, *_shader_storage.get(), std::move(new_level), *_type_name_lookup);
        _token_store += _level->on_room_selected += [&](uint16_t room) { select_room(room); };
        _token_store += _level->on_alternate_mode_selected += [&](bool enabled) { set_alternate_mode(enabled); };
        _token_store += _level->on_alternate_group_selected += [&](uint16_t group, bool enabled) { set_alternate_group(group, enabled); };
        _token_store += _level->on_level_changed += [&]() { _scene_changed = true; };

        _items_windows->set_items(_level->items());
        _items_windows->set_triggers(_level->triggers());
        _triggers_windows->set_items(_level->items());
        _triggers_windows->set_triggers(_level->triggers());
        _route_window_manager->set_items(_level->items());
        _route_window_manager->set_triggers(_level->triggers());
        _route_window_manager->set_rooms(_level->rooms());
        _rooms_windows->set_items(_level->items());
        _rooms_windows->set_triggers(_level->triggers());
        _rooms_windows->set_rooms(_level->rooms());

        _level->set_show_triggers(_ui->show_triggers());
        _level->set_show_hidden_geometry(_ui->show_hidden_geometry());
        _level->set_show_water(_ui->show_water());
        _level->set_show_wireframe(_ui->show_wireframe());

        // Set up the views.
        auto rooms = _level->room_info();
        _camera.reset();

        // Reset UI buttons
        _ui->set_max_rooms(static_cast<uint32_t>(rooms.size()));
        _ui->set_highlight(false);
        _ui->set_use_alternate_groups(_level->version() >= trlevel::LevelVersion::Tomb4);
        _ui->set_alternate_groups(_level->alternate_groups());
        _ui->set_flip(false);
        _ui->set_flip_enabled(_level->any_alternates());

        Item lara;
        if (_settings.go_to_lara && find_item_by_type_id(*_level, 0u, lara))
        {
            select_item(lara);
        }
        else
        {
            select_room(0);
        }

        _ui->set_depth_enabled(false);
        _ui->set_depth_level(1);

        // Strip the last part of the path away.
        auto last_index = std::min(filename.find_last_of('\\'), filename.find_last_of('/'));
        auto name = last_index == filename.npos ? filename : filename.substr(std::min(last_index + 1, filename.size()));
        _ui->set_level(name, _level->version());
        window().set_title("trview - " + name);
        _measure->reset();
        _route->clear();
        _route_window_manager->set_route(_route.get());

        _recent_orbits.clear();
        _recent_orbit_index = 0u;

        _scene_changed = true;
        */
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

            _viewer.render();
            Sleep(1);
        }

        return (int)msg.wParam;
    }
}
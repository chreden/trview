#include "Application.h"
#include <trlevel/LevelEncryptedException.h>
#include "UserCancelledException.h"
#include "Lua/trview/trview.h"
#include "Windows/IWindows.h"

#include "Resources/resource.h"
#include "Elements/SoundSource/ISoundSource.h"

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        void load_default_fonts(IFonts& fonts)
        {
            fonts.add_font("Default", { .name = "Arial", .filename = "arial.ttf", .size = 12 });
            fonts.add_font("Console", { .name = "Consolas", .filename = "consola.ttf", .size = 12 });
        }

        template <typename T>
        std::tuple<std::shared_ptr<T>, std::shared_ptr<ILevel>> get_entity_and_level(const std::weak_ptr<T>& entity)
        {
            if (const auto entity_ptr = entity.lock())
            {
                const auto level_ptr = entity_ptr->level().lock();
                return { entity_ptr, level_ptr };
            }
            return { nullptr, nullptr };
        }
    }

    IApplication::~IApplication()
    {
    }

    Application::Application(const Window& application_window,
        std::unique_ptr<IUpdateChecker> update_checker,
        std::shared_ptr<ISettingsLoader> settings_loader,
        std::unique_ptr<IFileMenu> file_menu,
        std::shared_ptr<IViewer> viewer,
        const IRoute::Source& route_source,
        std::shared_ptr<IShortcuts> shortcuts,
        const ILevel::Source& level_source,
        std::shared_ptr<IStartupOptions> startup_options,
        std::shared_ptr<IDialogs> dialogs,
        std::shared_ptr<IFiles> files,
        std::shared_ptr<IImGuiBackend> imgui_backend,
        std::shared_ptr<IPlugins> plugins,
        const IRandomizerRoute::Source& randomizer_route_source,
        std::shared_ptr<IFonts> fonts,
        std::unique_ptr<IWindows> windows,
        LoadMode load_mode)
        : MessageHandler(application_window), _instance(GetModuleHandle(nullptr)),
        _file_menu(std::move(file_menu)), _update_checker(std::move(update_checker)), _view_menu(window()), _settings_loader(settings_loader), _viewer(viewer),
        _route_source(route_source), _shortcuts(shortcuts), _level_source(level_source), _dialogs(dialogs), _files(files), _timer(default_time_source()),
        _imgui_backend(std::move(imgui_backend)), _plugins(plugins), _randomizer_route_source(randomizer_route_source), _fonts(fonts), _load_mode(load_mode),
        _windows(std::move(windows))
    {
        SetWindowLongPtr(window(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(_imgui_backend.get()));

        _update_checker->check_for_updates();
        _settings = _settings_loader->load_user_settings();
        lua::set_settings(_settings);

        set_route(_settings.randomizer_tools ? randomizer_route_source(std::nullopt) : route_source(std::nullopt));

        _file_menu->set_recent_files(_settings.recent_files);
        _token_store += _file_menu->on_file_open += [=](const auto& file) { open(file, ILevel::OpenMode::Full); };
        _token_store += _file_menu->on_reload += [=]() { reload(); };

        setup_shortcuts();
        setup_view_menu();

        _token_store += _windows->on_camera_sink_selected += [this](const auto& sink) {  select_camera_sink(sink); };
        _token_store += _windows->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
        _token_store += _windows->on_scene_changed += [this]() { _viewer->set_scene_changed(); };
        _token_store += _windows->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _windows->on_light_selected += [this](const auto& light) { select_light(light); };
        _token_store += _windows->on_room_selected += [this](const auto& room) { select_room(room); };
        _token_store += _windows->on_sector_hover += [this](const auto& sector) { select_sector(sector); };
        _token_store += _windows->on_waypoint_selected += [&](const auto& waypoint) { select_waypoint(waypoint); };
        _token_store += _windows->on_route_open += [&]() { this->open_route(); };
        _token_store += _windows->on_route_reload += [&]() { this->reload_route(); };
        _token_store += _windows->on_route_save += [&]() { this->save_route(); };
        _token_store += _windows->on_route_save_as += [&]() { this->save_route_as(); };
        _token_store += _windows->on_route_window_created += [&]() { open_recent_route(); };
        _token_store += _windows->on_level_switch += [&](const auto& level) { _file_menu->switch_to(level); };
        _token_store += _windows->on_new_route += [&]() { if (should_discard_changes()) { set_route(_route_source(std::nullopt)); } };
        _token_store += _windows->on_new_randomizer_route += [&]() { if (should_discard_changes()) { set_route(_randomizer_route_source(std::nullopt)); } };
        _token_store += _windows->on_static_selected += [this](const auto& stat) { select_static_mesh(stat); };
        _token_store += _windows->on_sound_source_selected += [this](const auto& sound) { select_sound_source(sound); };
        _token_store += _windows->on_diff_ended += [this](auto&& level) { end_diff(level); };
        _token_store += _windows->on_sector_selected += [this](auto&& sector)
            {
                if (const auto s = sector.lock())
                {
                    if (const auto r = s->room().lock())
                    {
                        select_room(r);
                        _viewer->set_target(r->sector_centroid(s));
                    }
                }
            };
        _token_store += _windows->on_settings += [this](auto&& settings)
            {
                _settings = settings;
                _viewer->set_settings(_settings);
                _windows->set_settings(settings);
                lua::set_settings(settings);
                if (_level)
                {
                    _level->set_map_colours(settings.map_colours);
                }
            };

        _windows->setup(_settings);
        setup_viewer(*startup_options);
        _plugins->initialise(this);
    }

    Application::~Application()
    {
        SetWindowLongPtr(window(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(nullptr));
        _settings_loader->save_user_settings(_settings);
        if (_imgui_backend->is_setup())
        {
            _imgui_backend->shutdown();
            if (ImGui::GetCurrentContext())
            {
                ImGui::DestroyContext();
            }
        }
    }

    void Application::open(const std::string& filename, ILevel::OpenMode open_mode)
    {
        if (open_mode == ILevel::OpenMode::Full && !is_rando_route() && !should_discard_changes())
        {
            return;
        }

        _load = std::async(std::launch::async, [=]() -> LoadOperation
            { 
                LoadOperation operation
                {
                    .filename = filename,
                    .open_mode = open_mode
                };

                try
                {
                    operation.level = load(filename);
                }
                catch (trlevel::LevelEncryptedException&)
                {
                    operation.error = "Level is encrypted and cannot be loaded";
                }
                catch (UserCancelledException&)
                {
                }
                catch (std::exception& e)
                {
                    operation.error = std::format("Failed to load level : {}", e.what());
                }

                return operation;
            });

        if (_load_mode == LoadMode::Sync)
        {
            _load.wait();
            check_load();
        }
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
                    case ID_WINDOWS_RESET_LAYOUT:
                    {
                        _imgui_backend->reset_layout();
                        break;
                    }
                    case ID_WINDOWS_RESET_FONTS:
                    {
                        load_default_fonts(*_fonts);
                        _settings.fonts = _fonts->fonts();
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
                    return (int)msg.wParam;
                }

                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }

            save_window_placement();
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
        _token_store += _view_menu.on_unhide_all += [&]()
        {
            if (!_level)
            {
                return;
            }

            for (const auto& item : _level->items()) { set_item_visibility(item, true); }
            for (const auto& trigger : _level->triggers()) { set_trigger_visibility(trigger, true); }
            for (const auto& light : _level->lights()) { set_light_visibility(light, true); }
            for (const auto& room : _level->rooms()) { set_room_visibility(room, true); }
            for (const auto& camera_sink : _level->camera_sinks()) { set_camera_sink_visibility(camera_sink, true); }
            for (const auto& static_mesh : _level->static_meshes()) { if (auto stat = static_mesh.lock()) { stat->set_visible(true); } };
            for (const auto& sound_source : _level->sound_sources()) { if (auto sound_source_ptr = sound_source.lock()) { sound_source_ptr->set_visible(true); } };
        };
    }

    void Application::setup_viewer(const IStartupOptions& startup_options)
    {
        _token_store += _viewer->on_item_visibility += [this](const auto& item, bool value) { set_item_visibility(item, value); };
        _token_store += _viewer->on_item_selected += [this](const auto& item) { select_item(item); };
        _token_store += _viewer->on_room_selected += [this](const auto& room) { select_room(room); };
        _token_store += _viewer->on_trigger_selected += [this](const auto& trigger) { select_trigger(trigger); };
        _token_store += _viewer->on_trigger_visibility += [this](const auto& trigger, bool value) { set_trigger_visibility(trigger, value); };
        _token_store += _viewer->on_light_selected += [this](const auto& light) { select_light(light); };
        _token_store += _viewer->on_light_visibility += [this](const auto& light, bool value) { set_light_visibility(light, value); };
        _token_store += _viewer->on_room_visibility += [this](const auto& room, bool value) { set_room_visibility(room, value); };
        _token_store += _viewer->on_camera_sink_visibility += [this](const auto& camera_sink, bool value) { set_camera_sink_visibility(camera_sink, value); };
        _token_store += _viewer->on_waypoint_added += [this](const auto& position, const auto& normal, auto room, auto type, auto index) { add_waypoint(position, normal, room, type, index); };
        _token_store += _viewer->on_waypoint_selected += [this](auto index) { select_waypoint(index); };
        _token_store += _viewer->on_waypoint_removed += [this](auto index) { remove_waypoint(index); };
        _token_store += _viewer->on_camera_sink_selected += [this](const auto& camera_sink) { select_camera_sink(camera_sink); };
        _token_store += _viewer->on_settings += [this](auto&& settings)
        {
            _settings = settings;
            _viewer->set_settings(_settings);
            _windows->set_settings(settings);
            lua::set_settings(settings);
            if (_level)
            {
                _level->set_map_colours(settings.map_colours);
            }
        };
        _token_store += _viewer->on_font += [this](auto&& name, auto&& font) { _new_font = { name, font }; };
        _token_store += _viewer->on_static_mesh_selected += [this](const auto& static_mesh) { select_static_mesh(static_mesh); };
        _token_store += _viewer->on_sound_source_selected += [this](const auto& sound_source) { select_sound_source(sound_source); };

        _viewer->set_settings(_settings);

        auto filename = startup_options.filename();
        if (!filename.empty())
        {
            open(filename, ILevel::OpenMode::Full);
        }
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

    void Application::add_waypoint(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& normal, std::weak_ptr<IRoom> room, IWaypoint::Type type, uint32_t index)
    {
        const auto room_ptr = room.lock();
        add_waypoint(position, normal, room_ptr ? room_ptr->number() : 0u, type, index);
    }

    void Application::add_waypoint(const Vector3& position, const Vector3& normal, uint32_t room, IWaypoint::Type type, uint32_t index)
    {
        uint32_t new_index = _route->insert(position, normal, room, type, index);
        select_waypoint(_route->waypoint(new_index));
    }

    void Application::remove_waypoint(uint32_t index)
    {
        _route->remove(index);
        _viewer->set_route(_route);

        if (_route->waypoints() > 0)
        {
            select_waypoint(_route->waypoint(_route->selected_waypoint()));
        }
    }

    void Application::select_item(std::weak_ptr<IItem> item)
    {
        const auto [item_ptr, level] = get_entity_and_level(item);
        if (!item_ptr || !level)
        {
            return;
        }

        _viewer->open(level, ILevel::OpenMode::Reload);
        select_room(item_ptr->room());
        level->set_selected_item(item);
        _viewer->select_item(item);
        _windows->select(item);
    }

    void Application::select_room(std::weak_ptr<IRoom> room)
    {
        const auto [room_ptr, level] = get_entity_and_level(room);
        if (!room_ptr || !level)
        {
            return;
        }

        level->set_selected_room(room);
        _viewer->open(level, ILevel::OpenMode::Reload);
        _viewer->select_room(room);
        _windows->set_room(room);
    }

    void Application::select_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        const auto [trigger_ptr, level] = get_entity_and_level(trigger);
        if (!trigger_ptr || !level)
        {
            return;
        }

        _viewer->open(level, ILevel::OpenMode::Reload);
        select_room(trigger_ptr->room());
        level->set_selected_trigger(trigger_ptr->number());
        _viewer->select_trigger(trigger);
        _windows->select(trigger);
    }

    void Application::select_waypoint(const std::weak_ptr<IWaypoint>& waypoint)
    {
        if (!_level)
        {
            return;
        }

        if (auto waypoint_ptr = waypoint.lock())
        {
            select_room(_level->room(waypoint_ptr->room()));
            _route->select_waypoint(waypoint_ptr);
            _viewer->select_waypoint(waypoint_ptr);
            _windows->select(waypoint_ptr);
        }
    }

    void Application::select_next_waypoint()
    {
        if (_route->selected_waypoint() + 1 < _route->waypoints())
        {
            select_waypoint(_route->waypoint(_route->selected_waypoint() + 1));
        }
    }

    void Application::select_previous_waypoint()
    {
        if (_route->selected_waypoint() > 0)
        {
            select_waypoint(_route->waypoint(_route->selected_waypoint() - 1));
        }
    }

    void Application::select_light(const std::weak_ptr<ILight>& light)
    {
        const auto [light_ptr, level] = get_entity_and_level(light);
        if (!light_ptr || !level)
        {
            return;
        }

        _viewer->open(level, ILevel::OpenMode::Reload);
        select_room(light_ptr->room());
        level->set_selected_light(light_ptr->number());
        _viewer->select_light(light);
        _windows->select(light);
    }

    void Application::set_item_visibility(const std::weak_ptr<IItem>& item, bool visible)
    {
        if (!_level)
        {
            return;
        }

        if (const auto item_ptr = item.lock())
        {
            if (item_ptr->visible() != visible)
            {
                _level->set_item_visibility(item_ptr->number(), visible);
            }
        }
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
            }
        }
    }

    void Application::set_light_visibility(const std::weak_ptr<ILight>& light, bool visible)
    {
        if (!_level)
        {
            return;
        }

        if (const auto light_ptr = light.lock())
        {
            if (light_ptr->visible() != visible)
            {
                _level->set_light_visibility(light_ptr->number(), visible);
            }
        }
    }

    void Application::set_room_visibility(const std::weak_ptr<IRoom>& room, bool visible)
    {
        if (!_level)
        {
            return;
        }

        if (const auto room_ptr = room.lock())
        {
            if (room_ptr->visible() != visible)
            {
                _level->set_room_visibility(room_ptr->number(), visible);
            }
        }
    }

    void Application::set_camera_sink_visibility(const std::weak_ptr<ICameraSink>& camera_sink, bool visible)
    {
        if (!_level)
        {
            return;
        }

        if (const auto camera_sink_ptr = camera_sink.lock())
        {
            if (camera_sink_ptr->visible() != visible)
            {
                _level->set_camera_sink_visibility(camera_sink_ptr->number(), visible);
            }
        }
    }

    void Application::select_sector(const std::weak_ptr<ISector>& sector)
    {
        _viewer->select_sector(sector);
    }

    void Application::render()
    {
        // If minimised, don't render like crazy. Sleep so we don't hammer the CPU either.
        if (window_is_minimised(window()))
        {
            Sleep(1);
            return;
        }

        if (!_imgui_backend->is_setup())
        {
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            auto existing_context = ImGui::GetCurrentContext();
            if (existing_context)
            {
                ImGui::DestroyContext(existing_context);
            }
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO();
            io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
            io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

            _imgui_ini_filename = _files->appdata_directory() + "\\trview\\imgui.ini";
            io.IniFilename = _imgui_ini_filename.c_str();

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();

            // Setup Platform/Renderer backends
            _imgui_backend->initialise();

            for (const auto& font : _settings.fonts)
            {
                _fonts->add_font(font.first, font.second);
            }
        }

        check_load();

        _timer.update();
        const auto elapsed = _timer.elapsed();
        _windows->update(elapsed);

        _viewer->render();

        if (_new_font.has_value())
        {
            if (_fonts->add_font(_new_font->first, _new_font->second))
            {
                _settings.fonts[_new_font->first] = _new_font->second;
                _viewer->set_settings(_settings);
            }
            _new_font.reset();
        }

        _imgui_backend->new_frame();
        ImGui::NewFrame();

        ImGui::PushFont(_fonts->font("Default"));


        if (_load.valid())
        {
            const auto viewport = ImGui::GetMainViewport();
            const ImVec2 size = ImGui::CalcTextSize(_progress.c_str());
            ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x + viewport->Size.x * 0.75f, viewport->Pos.y), 0, ImVec2(0.5f, 0.0f));
            ImGui::SetNextWindowSize(ImVec2(400, size.y));
            if (ImGui::Begin("Load Progress", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
            {
                ImGui::Text(_progress.c_str());
                ImGui::End();
            }
        }

        _viewer->render_ui();
        _windows->render();
        _plugins->render_ui();

        ImGui::PopFont();
        ImGui::Render();
        _imgui_backend->render();

        // Update and Render additional Platform Windows
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        _viewer->present(_settings.vsync);
    }

    bool Application::is_rando_route() const
    {
        return std::dynamic_pointer_cast<IRandomizerRoute>(_route) != nullptr;
    }

    bool Application::should_discard_changes()
    {
        if (_route->is_unsaved())
        {
            return _dialogs->message_box(L"Route has unsaved changes. Do you want to continue and lose changes?", L"Unsaved Route Changes", IDialogs::Buttons::Yes_No);
        }
        return true;
    }

    void Application::reload()
    {
        if (!_level)
        {
            return;
        }
        open(_level->filename(), ILevel::OpenMode::Reload);
    }

    void Application::open_route()
    {
        std::vector<IDialogs::FileFilter> filters{ { L"trview route", { L"*.tvr" } } };
        if (_settings.randomizer_tools)
        {
            filters.push_back({ L"Randomizer Locations", { L"*.json" } });
        }

        const auto filename = _dialogs->open_file(L"Import route", filters, OFN_FILEMUSTEXIST);
        if (filename)
        {
            import_route(filename->filename,
                filename->filter_index == 2 ||
                filename->filename.ends_with(".json"));
        }
    }

    void Application::reload_route()
    {
        try
        {
            _route->reload(_files, _settings);
        }
        catch (std::exception& e)
        {
            MessageBoxA(0, e.what(), "Error", MB_OK);
        }
    }

    void Application::save_route()
    {
        if (!_route->filename())
        {
            save_route_as();
        }
        else
        {
            _route->save(_files, _settings);
        }
    }

    void Application::import_route(const std::string& path, bool is_rando)
    {
        auto route = is_rando ?
            trview::import_randomizer_route(_randomizer_route_source, _files, path, _settings.randomizer) :
            trview::import_route(_route_source, _files, path);
        if (route)
        {
            route->set_filename(path);
            route->set_unsaved(false);
            set_route(route);
            if (_level)
            {
                _settings.recent_routes[_level->filename()] = { path, is_rando };
                _viewer->set_settings(_settings);
            }
        }
    }

    void Application::save_route_as()
    {
        const bool is_rando = std::dynamic_pointer_cast<IRandomizerRoute>(_route) != nullptr;
        std::vector<IDialogs::FileFilter> filters;
        if (is_rando)
        {
            filters.push_back({ L"Randomizer Locations", { L"*.json" }});
        }
        else
        {
            filters.push_back({ L"trview route", { L"*.tvr" } });
        }

        const auto filename = _dialogs->save_file(L"Save route", filters, 1);
        if (filename.has_value())
        {
            try
            {
                _route->save_as(_files, filename->filename, _settings);
                _route->set_unsaved(false);
                _route->set_filename(filename->filename);
                if (_level)
                {
                    _settings.recent_routes[_level->filename()] = { filename->filename, is_rando };
                    _viewer->set_settings(_settings);
                }
            }
            catch (std::exception& e)
            {
                MessageBoxA(0, e.what(), "Error saving route", MB_OK);
            }
        }
    }

    void Application::open_recent_route()
    {
        if (!_level || _recent_route_prompted  || !_windows->is_route_window_open() || std::dynamic_pointer_cast<IRandomizerRoute>(_route) != nullptr)
        {
            return;
        }

        const auto recent_route = _settings.recent_routes.find(_level->filename());
        if (recent_route != _settings.recent_routes.end() &&
            _dialogs->message_box(L"Reopen last used route for this level?", L"Reopen route", IDialogs::Buttons::Yes_No))
        {
            this->import_route(recent_route->second.route_path, recent_route->second.is_rando);
        }
        else
        {
            _settings.recent_routes.erase(_level->filename());
            _viewer->set_settings(_settings);
        }
        _recent_route_prompted = true;
    }

    void Application::save_window_placement()
    {
        WINDOWPLACEMENT placement{};
        placement.length = sizeof(placement);
        if (GetWindowPlacement(window(), &placement))
        {
            _settings.window_placement =
            {
                placement.ptMinPosition.x, placement.ptMinPosition.y,
                placement.ptMaxPosition.x, placement.ptMaxPosition.y,
                placement.rcNormalPosition.left, placement.rcNormalPosition.top, placement.rcNormalPosition.right, placement.rcNormalPosition.bottom
            };
        }
    }

    void Application::select_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        const auto [camera_sink_ptr, level] = get_entity_and_level(camera_sink);
        if (!camera_sink_ptr || !level)
        {
            return;
        }

        _viewer->open(level, ILevel::OpenMode::Reload);
        select_room(actual_room(*camera_sink_ptr));
        level->set_selected_camera_sink(camera_sink_ptr->number());
        _viewer->select_camera_sink(camera_sink);
        _windows->select(camera_sink);
    }

    std::weak_ptr<ILevel> Application::current_level() const
    {
        return _level;
    }

    std::shared_ptr<ILevel> Application::load(const std::string& filename)
    {
        _progress = std::format("Loading {}", filename);
        auto level = _level_source(filename, 
            {
                .on_progress_callback = [&](auto&& p) { _progress = p; }
            });
        level->set_filename(filename);
        return level;
    }

    void Application::set_current_level(const std::shared_ptr<ILevel>& level, ILevel::OpenMode open_mode, bool prompt_user)
    {
        if (level == _level)
        {
            return;
        }

        if (prompt_user && open_mode == ILevel::OpenMode::Full && !is_rando_route() && !should_discard_changes())
        {
            throw UserCancelledException();
        }

        auto old_level = _level;
        _level = level;

        _file_menu->open_file(level->filename());
        _level->set_map_colours(_settings.map_colours);
        _windows->set_level(_level);
        if (open_mode == ILevel::OpenMode::Full)
        {
            _route->clear();
            _route->set_colour(_settings.route_colour);
            _route->set_waypoint_colour(_settings.waypoint_colour);
            if (!is_rando_route())
            {
                _route->set_unsaved(false);
            }
        }

        set_route(_route);
        _viewer->open(level, open_mode);

        if (old_level && open_mode == ILevel::OpenMode::Reload)
        {
            const Vector3 old_target = _viewer->target();
            const bool old_auto_orbit = _settings.auto_orbit;
            _settings.auto_orbit = false;
            _viewer->set_settings(_settings);

            auto selected_item = old_level->selected_item();
            if (selected_item.has_value())
            {
                if (const auto new_selected_item = _level->item(selected_item.value()).lock())
                {
                    select_item(new_selected_item);
                }
            }

            auto selected_trigger = old_level->selected_trigger();
            if (selected_trigger.has_value())
            {
                select_trigger(_level->trigger(selected_trigger.value()));
            }

            auto selected_light = old_level->selected_light();
            if (selected_light.has_value())
            {
                select_light(_level->light(selected_light.value()));
            }

            auto selected_camera_sink = old_level->selected_camera_sink();
            if (selected_camera_sink.has_value())
            {
                select_camera_sink(_level->camera_sink(selected_camera_sink.value()));
            }

            if (auto selected_room = old_level->selected_room().lock())
            {
                select_room(_level->room(selected_room->number()));
            }

            _viewer->set_target(old_target);
            _settings.auto_orbit = old_auto_orbit;
            _viewer->set_settings(_settings);
        }
        else
        {
            _recent_route_prompted = false;
            open_recent_route();
        }
    }

    UserSettings Application::settings() const
    {
        return _settings;
    }

    std::weak_ptr<IViewer> Application::viewer() const
    {
        return _viewer;
    }

    std::vector<std::string> Application::local_levels() const
    {
        return _file_menu->local_levels();
    }

    std::shared_ptr<IRoute> Application::route() const
    {
        return _route;
    }

    void Application::set_route(const std::shared_ptr<IRoute>& route)
    {
        _route = route;
        _token_store += _route->on_changed += [&]() { if (_viewer) { _viewer->set_scene_changed(); } };
        _token_store += _route->on_waypoint_selected += [&](auto&& r) { select_waypoint(r); };
        _route->set_level(_level);
        _viewer->set_route(_route);
        _windows->set_route(_route);
    }

    void Application::select_static_mesh(const std::weak_ptr<IStaticMesh>& static_mesh)
    {
        const auto [static_mesh_ptr, level] = get_entity_and_level(static_mesh);
        if (!static_mesh_ptr || !level)
        {
            return;
        }

        _viewer->open(level, ILevel::OpenMode::Reload);
        select_room(static_mesh_ptr->room());
        _viewer->select_static_mesh(static_mesh_ptr);
        _windows->select(static_mesh_ptr);
    }

    void Application::select_sound_source(const std::weak_ptr<ISoundSource>& sound_source)
    {
        const auto [sound_source_ptr, level] = get_entity_and_level(sound_source);
        if (!sound_source_ptr || !level)
        {
            return;
        }

        _viewer->open(level, ILevel::OpenMode::Reload);
        _viewer->select_sound_source(sound_source);
        _windows->select(sound_source);
    }

    void Application::check_load()
    {
        if (!_load.valid() || _load.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
        {
            return;
        }

        const auto op = _load.get();
        _load = {};

        if (op.error)
        {
            _dialogs->message_box(to_utf16(op.error.value()), L"Error", IDialogs::Buttons::OK);
            return;
        }

        if (!op.level)
        {
            return;
        }

        _settings.add_recent_file(op.filename);
        _file_menu->set_recent_files(_settings.recent_files);
        _settings_loader->save_user_settings(_settings);
        _viewer->set_settings(_settings);
        set_current_level(op.level, op.open_mode, false);
    }

    void Application::end_diff(const std::weak_ptr<ILevel>& level)
    {
        const auto closed_level = level.lock();
        const auto open_level = _viewer->level().lock();
        if (closed_level == open_level)
        {
            _viewer->open(_level, ILevel::OpenMode::Reload);
        }
    }
}
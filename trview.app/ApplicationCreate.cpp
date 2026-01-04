#include "Application.h"

#include <trlevel/Level.h>
#include <trlevel/Decrypter.h>
#include <trlevel/Pack.h>
#include <trlevel/Hasher.h>
#include <trview.common/Files.h>
#include <trview.common/Logs/Log.h>
#include <trview.common/windows/Clipboard.h>
#include <trview.common/Windows/Dialogs.h>
#include <trview.common/Windows/Shell.h>

#include <trview.graphics/Sprite.h>
#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.graphics/RenderTarget.h>
#include <trview.graphics/Buffer.h>
#include <trview.graphics/Sampler/SamplerState.h>
#include <trview.input/WindowTester.h>

#include "Resources/resource.h"
#include "Resources/DefaultShaders.h"
#include "Resources/DefaultTextures.h"

#include "Elements/TypeInfoLookup.h"
#include "Elements/CameraSink/CameraSink.h"
#include "Elements/Flyby/Flyby.h"
#include "Elements/Flyby/FlybyNode.h"
#include "Elements/Item.h"
#include "Elements/Light.h"
#include "Elements/Trigger.h"
#include "Elements/Remastered/NgPlusSwitcher.h"
#include "Elements/StaticMesh.h"
#include "Elements/Sector.h"
#include "Elements/SoundSource/SoundSource.h"
#include "Elements/Level.h"
#include "Graphics/TextureStorage.h"
#include "Geometry/Mesh.h"
#include "Geometry/Picking.h"
#include "Geometry/TransparencyBuffer.h"
#include "Geometry/Model/Model.h"
#include "Geometry/Model/ModelStorage.h"
#include "Graphics/LevelTextureStorage.h"
#include "Graphics/MeshStorage.h"
#include "Graphics/SelectionRenderer.h"
#include "Graphics/SectorHighlight.h"
#include "Lua/Scriptable/Scriptable.h"
#include "Lua/Lua.h"
#include "Menus/FileMenu.h"
#include "Menus/ImGuiFileMenu.h"
#include "Menus/UpdateChecker.h"
#include "Routing/Waypoint.h"
#include "Routing/RandomizerRoute.h"
#include "Routing/Route.h"
#include "Settings/SettingsLoader.h"
#include "Settings/StartupOptions.h"
#include "Sound/SoundStorage.h"
#include "UI/CameraControls.h"
#include "UI/ContextMenu.h"
#include "UI/SettingsWindow.h"
#include "UI/ViewerUI.h"
#include "UI/ViewOptions.h"
#include "UI/MapRenderer.h"
#include "Windows/ItemsWindow.h"
#include "Windows/LightsWindow.h"
#include "Windows/RoomsWindow.h"
#include "Windows/RouteWindow.h"
#include "Windows/TriggersWindow.h"
#include "Windows/Viewer.h"
#include "Windows/Log/LogWindow.h"
#include "Windows/Models/ModelsWindow.h"
#include "UI/DX11ImGuiBackend.h"
#include "Windows/Textures/TexturesWindow.h"
#include "Windows/CameraSink/CameraSinkWindow.h"
#include "Windows/Console/Console.h"
#include "Plugins/Plugins.h"
#include "Plugins/Plugin.h"
#include "Windows/Plugins/PluginsWindow.h"
#include "Tools/Toolbar.h"
#include "UI/Fonts/Fonts.h"
#include "Windows/Sounds/SoundsWindow.h"
#include "Windows/Statics/StaticsWindow.h"
#include "Windows/Windows.h"
#include "Windows/About/AboutWindow.h"
#include "Windows/Diff/DiffWindow.h"
#include "Windows/Pack/PackWindow.h"
#include "UI/LevelInfo.h"
#include "Elements/Level/LevelNameLookup.h"

#include <trview.common/Messages/MessageSystem.h>
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    namespace
    {
        const std::wstring window_class{ L"TRVIEW" };
        const std::wstring window_title{ L"trview" };

        LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
        {
            LONG_PTR ptr = GetWindowLongPtr(hWnd, GWLP_USERDATA);
            IImGuiBackend* backend = reinterpret_cast<IImGuiBackend*>(ptr);
            if (backend && backend->window_procedure(hWnd, message, wParam, lParam))
            {
                return true;
            }
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

        Window create_window(HINSTANCE hInstance, int nCmdShow, const UserSettings& settings)
        {
            register_class(hInstance);

            HWND window = CreateWindowW(window_class.c_str(), window_title.c_str(), WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

            if (!window)
            {
                return nullptr;
            }

            if (settings.window_placement)
            {
                const auto p = settings.window_placement.value();
                WINDOWPLACEMENT placement{};
                placement.length = sizeof(placement);
                placement.showCmd = SW_SHOW;
                placement.ptMinPosition = { p.min_x, p.min_y };
                placement.ptMaxPosition = { p.max_x, p.max_y };
                placement.rcNormalPosition = { p.normal_left, p.normal_top, p.normal_right, p.normal_bottom };
                SetWindowPlacement(window, &placement);
            }
            else
            {
                ShowWindow(window, nCmdShow);
            }

            UpdateWindow(window);

            return window;
        }
    }

    std::shared_ptr<IApplication> create_application(HINSTANCE hInstance, int command_show, const std::wstring& command_line)
    {
        auto messaging = std::make_shared<MessageSystem>();
        auto files = std::make_shared<Files>();
        auto settings_loader = std::make_shared<SettingsLoader>(files);
        auto window = create_window(hInstance, command_show, settings_loader->load_user_settings());

        auto device = std::make_shared<graphics::Device>();
        auto shortcuts = std::make_shared<Shortcuts>(window);
        auto texture_storage = std::make_shared<TextureStorage>(device);
        auto shader_storage = std::make_shared<graphics::ShaderStorage>();

        Resource type_list = get_resource_memory(IDR_TYPE_NAMES, L"TEXT");
        auto extra_type_info = files->load_file(files->appdata_directory() + "\\trview\\types.json");
        auto type_info_lookup = std::make_shared<TypeInfoLookup>(
            std::string(type_list.data, type_list.data + type_list.size),
            extra_type_info.has_value() ? std::optional<std::string>(extra_type_info.value() | std::ranges::to<std::string>()) : std::nullopt);

        load_default_shaders(device, shader_storage);
        load_default_textures(device, texture_storage);

        auto render_target_source = [=](auto&&... args) { return std::make_unique<graphics::RenderTarget>(device, args...); };
        auto device_window_source = [=](auto&& window)
        {
            return std::make_unique<graphics::DeviceWindow>(device, render_target_source, window);
        };

        auto sprite_source = [=](auto&& size)
        {
            return std::make_unique<graphics::Sprite>(device, shader_storage, size);
        };

        auto default_mesh_source = [=](auto&&... args) { return std::make_shared<Mesh>(device, args..., texture_storage); };
        
        const auto waypoint_mesh = create_cube_mesh(default_mesh_source);
        auto waypoint_source = [=](auto&&... args) { return std::make_shared<Waypoint>(waypoint_mesh, args...); };

        auto route_source = [=](std::optional<IRoute::FileData> data)
        {
            auto new_route = std::make_shared<Route>(
                std::make_unique<SelectionRenderer>(device, shader_storage, std::make_unique<TransparencyBuffer>(device, texture_storage), render_target_source),
                waypoint_source, settings_loader->load_user_settings());
            if (data)
            {
                new_route->import(data->data);
            }
            return new_route;
        };

        auto randomizer_route_source = [=](std::optional<IRandomizerRoute::FileData> data)
        {
            auto new_route = std::make_shared<RandomizerRoute>(
                route_source(std::nullopt),
                waypoint_source);
            if (data)
            {
                new_route->import(data->data, data->settings);
            }
            return new_route;
        };

        auto log = std::make_shared<Log>();

        auto bounding_mesh = create_cube_mesh(default_mesh_source);
        auto static_mesh_source = [=](auto&&... args) { return std::make_shared<StaticMesh>(args..., bounding_mesh); };
        auto static_mesh_position_source = [=](auto&&... args) { return std::make_shared<StaticMesh>(args...); };
        auto sector_source = [=](auto&&... args) { return std::make_shared<Sector>(std::forward<decltype(args)>(args)...); };

        const auto light_mesh = create_sphere_mesh(default_mesh_source, 24, 24);
        auto light_source = [=](auto&&... args) { return std::make_shared<Light>(light_mesh, args...); };
        auto buffer_source = [=](auto&&... args) { return std::make_unique<graphics::Buffer>(device, args...); };

        auto cube_mesh = create_cube_mesh(default_mesh_source);
        auto camera_sink_source = [=](auto&&... args) { return std::make_shared<CameraSink>(cube_mesh, texture_storage, args...); };

        const auto sound_source = [=](auto&&... args) { return create_sound(args...); };
        const auto sound_source_source = [=](auto&&... args) { return std::make_shared<SoundSource>(cube_mesh, texture_storage, args...); };

        const auto flyby_node_source = [=](auto&&... args) { return std::make_shared<FlybyNode>(args...); };
        const auto flyby_mesh = create_frustum_mesh(default_mesh_source);
        const auto flyby_source = [=](auto&& nodes, auto&& level)
            { 
                auto flyby = std::make_shared<Flyby>(flyby_mesh, level);
                flyby->initialise(flyby_node_source, default_mesh_source, nodes);
                return flyby;
            };

        auto decrypter = std::make_shared<trlevel::Decrypter>();
        auto hasher = std::make_shared<trlevel::Hasher>();

        auto trlevel_pack_source = [=](auto&&... args) { return std::make_shared<trlevel::Level>(args..., files, decrypter, log, hasher); };
        const auto pack_source = [=](auto&&... args)
            { 
                auto pack = std::make_shared<trlevel::Pack>(args..., trlevel_pack_source);
                pack->load();
                return pack;
            };
        auto trlevel_source = [=](auto&&... args) { return std::make_shared<trlevel::Level>(args..., files, decrypter, log, hasher, pack_source); };

        D3D11_SAMPLER_DESC sampler_desc;
        memset(&sampler_desc, 0, sizeof(sampler_desc));
        sampler_desc.Filter = settings_loader->load_user_settings().linear_filtering ? D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_POINT;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.MaxAnisotropy = 1;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
        auto wrap_sampler_state = std::make_shared<graphics::SamplerState>(device, device->create_sampler_state(sampler_desc));

        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        auto clamp_sampler_state = std::make_shared<graphics::SamplerState>(device, device->create_sampler_state(sampler_desc));

        auto sampler_source = [=](graphics::ISamplerState::AddressMode mode)
            {
                return mode == graphics::ISamplerState::AddressMode::Wrap ?
                    wrap_sampler_state : clamp_sampler_state;
            };

        auto level_source = [=](auto&& filename, auto&& pack, auto&& callbacks)
            {
                auto level = trlevel_source(filename, pack);
                auto level_texture_storage = std::make_shared<LevelTextureStorage>(device, std::make_unique<TextureStorage>(device));

                int count = 0;
                callbacks.on_textile_callback = [&](auto&& textile, auto&& width, auto&& height)
                    {
                        callbacks.on_progress(std::format("Loading texture {}", ++count));
                        level_texture_storage->add_textile(textile, width, height);
                    };

                auto sound_storage = std::make_shared<SoundStorage>(sound_source);
                callbacks.on_sound_callback = [&](auto&& sound_map, auto&& sound_details, auto&& sample_index, auto&& data)
                    {
                        callbacks.on_progress(std::format("Loading sound {}:{}:{}", sound_map, sound_details, sample_index));
                        sound_storage->add({ .sound_map = sound_map, .sound_details = sound_details, .sample_index = sample_index }, data);
                    };

                level->load(callbacks);
                level_texture_storage->load(level);

                auto mesh_source = [=](auto&&... args) { return std::make_shared<Mesh>(device, args..., level_texture_storage); };

                auto entity_source = [=](auto&& level, auto&& entity, auto&& index, auto&& triggers, auto&& model_storage, auto&& owning_level, auto&& room)
                    {
                        return std::make_shared<Item>(mesh_source, level, entity, model_storage, owning_level, index,
                            type_info_lookup->lookup(level.platform_and_version(), entity.TypeID, entity.Flags),
                            triggers,
                            room);
                    };

                auto ai_source = [=](auto&& level, auto&& entity, auto&& index, auto&& model_storage, auto&& owning_level, auto&& room)
                    {
                        return std::make_shared<Item>(mesh_source, level, entity, model_storage, owning_level, index, type_info_lookup->lookup(level.platform_and_version(), entity.type_id, entity.flags), std::vector<std::weak_ptr<ITrigger>>{}, room);
                    };

                auto ngplus = std::make_shared<NgPlusSwitcher>(entity_source);

                auto room_source = [=](const trlevel::ILevel& level, const trlevel::tr3_room& room,
                    const std::shared_ptr<ILevelTextureStorage>& texture_storage, const IMeshStorage& mesh_storage, uint32_t index, const std::weak_ptr<ILevel>& parent_level, uint32_t sector_base_index, const Activity& activity)
                    {
                        auto new_room = std::make_shared<Room>(room, mesh_source, texture_storage, index, parent_level, sampler_source);
                        new_room->initialise(level, room, mesh_storage, static_mesh_source, static_mesh_position_source, sector_source, sector_base_index, activity);
                        return new_room;
                    };
                auto trigger_source = [=](auto&&... args) { return std::make_shared<Trigger>(args..., mesh_source); };

                auto mesh_storage = std::make_shared<MeshStorage>(mesh_source, *level, *level_texture_storage);

                auto model_source = [=](auto&&... args) { return std::make_shared<Model>(args..., cube_mesh, texture_storage); };
                auto model_storage = std::make_shared<ModelStorage>(mesh_storage, model_source, *level);
                auto new_level = std::make_shared<Level>(
                    device, 
                    shader_storage, 
                    level_texture_storage,
                    std::make_unique<TransparencyBuffer>(device, level_texture_storage),
                    std::make_unique<SelectionRenderer>(device, shader_storage, std::make_unique<TransparencyBuffer>(device, level_texture_storage), render_target_source),
                    log,
                    buffer_source,
                    sound_storage,
                    ngplus,
                    clamp_sampler_state,
                    messaging);

                std::shared_ptr<ILevel> level_ptr = new_level;
                std::shared_ptr<IRecipient> rec_ptr = new_level;
                messaging->add_recipient(new_level);
                new_level->initialise(level,
                    mesh_storage,
                    model_storage,
                    entity_source,
                    ai_source,
                    room_source,
                    trigger_source,
                    light_source,
                    camera_sink_source,
                    sound_source_source,
                    flyby_source,
                    callbacks);

                return new_level;
            };

        auto scriptable_source = [=](auto state) { return std::make_shared<Scriptable>(state, create_cube_mesh(default_mesh_source), texture_storage->coloured(Colour::White)); };

        auto dialogs = std::make_shared<Dialogs>(window);
        auto shell = std::make_shared<Shell>();

        auto plugin_source = [=](auto&&... args) { return std::make_shared<Plugin>(files, std::make_unique<Lua>(route_source, randomizer_route_source, waypoint_source, scriptable_source, dialogs, files), args...); };
        auto plugins = std::make_shared<Plugins>(
            files,
            std::make_shared<Plugin>(std::make_unique<Lua>(route_source, randomizer_route_source, waypoint_source, scriptable_source, dialogs, files), "Default", "trview", "Default Lua plugin for trview"),
            plugin_source,
            settings_loader->load_user_settings());
        messaging->add_recipient(plugins);

        auto plugins_window_source = [=]() { return std::make_shared<PluginsWindow>(plugins, shell, dialogs, messaging); };
        auto imgui_backend = std::make_shared<DX11ImGuiBackend>(window, device, files);
        auto fonts = std::make_shared<Fonts>(files, imgui_backend);
        auto map_renderer_source = [=]()
            { 
                auto renderer = std::make_shared<MapRenderer>(fonts, messaging);
                messaging->add_recipient(renderer);
                renderer->initialise();
                return renderer;
            };
        auto clipboard = std::make_shared<Clipboard>(window);
        auto items_window_source = [=]()
            {
                auto new_window = std::make_shared<ItemsWindow>(clipboard, messaging);
                messaging->add_recipient(new_window);
                new_window->initialise();
                return new_window;
            };
        auto rooms_window_source = [=]()
            {
                auto new_window = std::make_shared<RoomsWindow>(map_renderer_source, clipboard, messaging);
                messaging->add_recipient(new_window);
                new_window->initialise();
                return new_window;
            };

        auto settings_window = std::make_shared<SettingsWindow>(dialogs, shell, fonts, texture_storage, messaging);
        messaging->add_recipient(settings_window);

        Resource level_hashes = get_resource_memory(IDR_LEVEL_HASHES, L"TEXT");
        auto level_name_lookup = std::make_shared<LevelNameLookup>(files, std::string(level_hashes.data, level_hashes.data + level_hashes.size));

        auto triggers_window_source = [=]()
            {
                auto triggers_window = std::make_shared<TriggersWindow>(clipboard, messaging);
                messaging->add_recipient(triggers_window);
                triggers_window->initialise();
                return triggers_window;
            };
        auto route_window_source = [=]()
            {
                auto route_window = std::make_shared<RouteWindow>(clipboard, dialogs, files, messaging);
                messaging->add_recipient(route_window);
                route_window->initialise();
                return route_window;
            };
        auto lights_window_source = [=]()
            {
                auto lights_window = std::make_shared<LightsWindow>(clipboard, messaging);
                messaging->add_recipient(lights_window);
                lights_window->initialise();
                return lights_window;
            };

        auto log_window_source = [=]() { return std::make_shared<LogWindow>(log, dialogs, files); };
        const auto camera = std::make_shared<Camera>(window.size());
        auto camera_sink_window_source = [=]()
            {
                auto camera_sink_window = std::make_shared<CameraSinkWindow>(clipboard, camera, messaging);
                messaging->add_recipient(camera_sink_window);
                camera_sink_window->initialise();
                return camera_sink_window;
            };

        auto textures_window_source = [=]()
            {
                auto textures_window = std::make_shared<TexturesWindow>(messaging);
                messaging->add_recipient(textures_window);
                textures_window->initialise();
                return textures_window;
            };
        auto console_source = [=]() { return std::make_shared<Console>(dialogs, plugins, fonts); };
        auto statics_window_source = [=]()
            {
                auto statics_window = std::make_shared<StaticsWindow>(clipboard, messaging);
                messaging->add_recipient(statics_window);
                statics_window->initialise();
                return statics_window;
            };
        auto sounds_window_source = [=]()
            {
                auto sounds_window = std::make_shared<SoundsWindow>(messaging);
                messaging->add_recipient(sounds_window);
                sounds_window->initialise();
                return sounds_window;
            };
        auto about_window_source = [=]() { return std::make_shared<AboutWindow>(); };

        auto level_name_source = [=](auto&& filename, auto&& pack) -> std::optional<ILevelNameLookup::Name>
            {
                try
                {
                    auto level = trlevel_source(filename, pack);
                    level->load(trlevel::ILevel::LoadCallbacks{ .open_mode = trlevel::ILevel::LoadCallbacks::OpenMode::Preview });
                    return level_name_lookup->lookup(level);
                }
                catch (...)
                {
                }
                return std::nullopt;
            };

        auto imgui_file_menu = std::make_shared<ImGuiFileMenu>(dialogs, files, level_name_source);
        messaging->add_recipient(imgui_file_menu);

        auto diff_window_source = [=]()
            {
                auto diff_window = std::make_shared<DiffWindow>(dialogs, level_source, imgui_file_menu, messaging);
                diff_window->initialise();
                return diff_window;
            };
        auto pack_window_source = [=]()
            {
                auto pack_window = std::make_shared<PackWindow>(files, dialogs, messaging);
                pack_window->initialise();
                return pack_window;
            };

        auto transparency_buffer_source = [=](auto&& lts) { return std::make_unique<TransparencyBuffer>(device, lts); };

        auto models_window_source = [=]()
            {
                auto models_window = std::make_shared<ModelsWindow>(device, render_target_source, shader_storage, buffer_source, transparency_buffer_source, sampler_source, std::make_unique<input::Mouse>(window, std::make_unique<input::WindowTester>(window)), messaging);
                models_window->initialise();
                return models_window;
            };

        auto windows = std::make_shared<Windows>(window, shortcuts);
        windows->register_window("About", about_window_source);
        windows->register_window("CameraSink", camera_sink_window_source);
        windows->register_window("Console", console_source);
        windows->register_window("Diff", diff_window_source);
        windows->register_window("Items", items_window_source);
        windows->register_window("Lights", lights_window_source);
        windows->register_window("Log", log_window_source);
        windows->register_window("Models", models_window_source);
        windows->register_window("Pack", pack_window_source);
        windows->register_window("Plugins", plugins_window_source);
        windows->register_window("Rooms", rooms_window_source);
        windows->register_window("Route", route_window_source);
        windows->register_window("Sounds", sounds_window_source);
        windows->register_window("Statics", statics_window_source);
        windows->register_window("Textures", textures_window_source);
        windows->register_window("Triggers", triggers_window_source);

        auto viewer_ui = std::make_shared<ViewerUI>(
            window,
            shortcuts,
            map_renderer_source,
            settings_window,
            std::make_unique<ViewOptions>(windows),
            std::make_unique<ContextMenu>(windows),
            std::make_unique<CameraControls>(),
            std::make_unique<Toolbar>(plugins),
            messaging,
            std::make_unique<LevelInfo>(*texture_storage, level_name_lookup));
        messaging->add_recipient(viewer_ui);

        auto viewer = std::make_shared<Viewer>(
            window,
            device,
            viewer_ui,
            std::make_unique<Picking>(window),
            std::make_unique<input::Mouse>(window, std::make_unique<input::WindowTester>(window)),
            shortcuts,
            route_source(std::nullopt),
            std::make_unique<Compass>(device, sprite_source, render_target_source, default_mesh_source),
            std::make_unique<Measure>(device, default_mesh_source),
            device_window_source,
            std::make_unique<SectorHighlight>(default_mesh_source),
            clipboard,
            camera,
            sampler_source,
            messaging);
        messaging->add_recipient(viewer);
        viewer->initialise();

        auto file_menu = std::make_shared<FileMenu>(window, shortcuts, dialogs, files, level_name_source, messaging);
        messaging->add_recipient(file_menu);

        auto application = std::make_shared<Application>(
            window,
            std::make_unique<UpdateChecker>(window),
            settings_loader,
            file_menu,
            viewer,
            route_source,
            shortcuts,
            level_source,
            std::make_shared<StartupOptions>(command_line),
            dialogs,
            files,
            imgui_backend,
            plugins,
            randomizer_route_source,
            fonts,
            windows,
            Application::LoadMode::Async,
            messaging);
        messaging->add_recipient(application);
        return application;
    }
}

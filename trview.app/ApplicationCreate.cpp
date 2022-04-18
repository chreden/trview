#include "Application.h"

#include <trlevel/Level.h>
#include <trlevel/Decrypter.h>
#include <trview.common/Files.h>
#include <trview.common/Logs/Log.h>
#include <trview.common/windows/Clipboard.h>
#include <trview.common/Windows/Dialogs.h>
#include <trview.common/Windows/Shell.h>

#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.graphics/RenderTarget.h>
#include <trview.graphics/Buffer.h>
#include <trview.input/WindowTester.h>

#include "Resources/resource.h"
#include "Resources/DefaultShaders.h"
#include "Resources/DefaultTextures.h"
#include "Resources/DefaultFonts.h"

#include "Elements/TypeInfoLookup.h"
#include "Elements/CameraSink/CameraSink.h"
#include "Elements/Item.h"
#include "Elements/Light.h"
#include "Elements/Trigger.h"
#include "Elements/StaticMesh.h"
#include "Elements/Sector.h"
#include "Graphics/TextureStorage.h"
#include "Geometry/Mesh.h"
#include "Geometry/Picking.h"
#include "Geometry/TransparencyBuffer.h"
#include "Graphics/LevelTextureStorage.h"
#include "Graphics/MeshStorage.h"
#include "Graphics/SelectionRenderer.h"
#include "Graphics/SectorHighlight.h"
#include "Menus/FileMenu.h"
#include "Menus/UpdateChecker.h"
#include "Routing/Waypoint.h"
#include "Routing/RandomizerRoute.h"
#include "Settings/SettingsLoader.h"
#include "Settings/StartupOptions.h"
#include "UI/CameraControls.h"
#include "UI/ContextMenu.h"
#include "UI/SettingsWindow.h"
#include "UI/ViewerUI.h"
#include "UI/ViewOptions.h"
#include "UI/MapRenderer.h"
#include "Windows/ItemsWindowManager.h"
#include "Windows/LightsWindow.h"
#include "Windows/LightsWindowManager.h"
#include "Windows/RouteWindowManager.h"
#include "Windows/RoomsWindowManager.h"
#include "Windows/TriggersWindowManager.h"
#include "Windows/Viewer.h"
#include "Windows/Log/LogWindow.h"
#include "Windows/Log/LogWindowManager.h"
#include "UI/DX11ImGuiBackend.h"
#include "Tools/Mover.h"
#include "Windows/Textures/TexturesWindowManager.h"
#include "Windows/Textures/TexturesWindow.h"
#include "Windows/CameraSink/CameraSinkWindowManager.h"
#include "Windows/CameraSink/CameraSinkWindow.h"
#include "Windows/Console/ConsoleManager.h"
#include "Windows/Console/Console.h"
#include "Plugins/Plugins.h"
#include "Plugins/Plugin.h"
#include "Windows/Plugins/PluginsWindowManager.h"
#include "Windows/Plugins/PluginsWindow.h"
#include "Tools/Toolbar.h"
#include "UI/Fonts/Fonts.h"
#include "Windows/Statics/StaticsWindowManager.h"
#include "Windows/Statics/StaticsWindow.h"

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

    std::unique_ptr<IApplication> create_application(HINSTANCE hInstance, int command_show, const std::wstring& command_line)
    {
        auto files = std::make_shared<Files>();
        auto settings_loader = std::make_shared<SettingsLoader>(files);
        auto window = create_window(hInstance, command_show, settings_loader->load_user_settings());

        auto device = std::make_shared<graphics::Device>();
        auto shortcuts = std::make_shared<Shortcuts>(window);
        auto texture_storage = std::make_shared<TextureStorage>(device);
        auto shader_storage = std::make_shared<graphics::ShaderStorage>();
        auto font_factory = std::make_shared<graphics::FontFactory>();

        Resource type_list = get_resource_memory(IDR_TYPE_NAMES, L"TEXT");
        auto type_info_lookup = std::make_shared<TypeInfoLookup>(std::string(type_list.data, type_list.data + type_list.size));

        load_default_shaders(device, shader_storage);
        load_default_fonts(device, font_factory);
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

        auto map_renderer_source = [=](auto&& size)
        {
            return std::make_unique<MapRenderer>(device, font_factory, size, sprite_source, render_target_source);
        };

        auto mesh_source = [=](auto&&... args) { return std::make_shared<Mesh>(device, args...); };
        auto mesh_transparent_source = [=](auto&&... args) { return std::make_shared<Mesh>(args...); };

        const auto waypoint_mesh = create_cube_mesh(mesh_source);
        auto waypoint_source = [=](auto&&... args) { return std::make_shared<Waypoint>(waypoint_mesh, args...); };

        auto route_source = [=](std::optional<IRoute::FileData> data)
        {
            auto new_route = std::make_shared<Route>(
                std::make_unique<SelectionRenderer>(device, shader_storage, std::make_unique<TransparencyBuffer>(device), render_target_source),
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

        auto entity_source = [=](auto&& level, auto&& entity, auto&& index, auto&& triggers, auto&& mesh_storage, auto&& owning_level, auto&& room)
        {
            return std::make_shared<Item>(mesh_source, level, entity, mesh_storage, owning_level, index,
                type_info_lookup->lookup(level.get_version(), entity.TypeID, entity.Flags),
                triggers,
                room);
        };

        auto ai_source = [=](auto&& level, auto&& entity, auto&& index, auto&& mesh_storage, auto&& owning_level, auto&& room)
        {
            return std::make_shared<Item>(mesh_source, level, entity, mesh_storage, owning_level, index, type_info_lookup->lookup(level.get_version(), entity.type_id, entity.flags), std::vector<std::weak_ptr<ITrigger>>{}, room);
        };

        auto log = std::make_shared<Log>();

        auto bounding_mesh = create_cube_mesh(mesh_source);
        auto static_mesh_source = [=](auto&&... args) { return std::make_shared<StaticMesh>(args..., bounding_mesh); };
        auto static_mesh_position_source = [=](auto&&... args) { return std::make_shared<StaticMesh>(args...); };
        auto sector_source = [=](auto&&... args) { return std::make_shared<Sector>(std::forward<decltype(args)>(args)...); };
        auto room_source = [=](const trlevel::ILevel& level, const trlevel::tr3_room& room,
            const std::shared_ptr<ILevelTextureStorage>& texture_storage, const IMeshStorage& mesh_storage, uint32_t index, const std::weak_ptr<ILevel>& parent_level, const Activity& activity)
        {
            auto new_room = std::make_shared<Room>(room, mesh_source, texture_storage, index, parent_level);
            new_room->initialise(level, room, mesh_storage, static_mesh_source, static_mesh_position_source, sector_source, activity);
            return new_room;
        };
        auto trigger_source = [=](auto&&... args) { return std::make_shared<Trigger>(args..., mesh_transparent_source); };

        const auto light_mesh = create_sphere_mesh(mesh_source, 24, 24);
        auto light_source = [=](auto&&... args) { return std::make_shared<Light>(light_mesh, args...); };
        auto buffer_source = [=](auto&&... args) { return std::make_unique<graphics::Buffer>(device, args...); };

        auto camera_mesh = create_cube_mesh(mesh_source);
        auto camera_sink_source = [=](auto&&... args) { return std::make_shared<CameraSink>(camera_mesh, texture_storage, args...); };

        auto level_source = [=](auto&& level)
        {
            auto level_texture_storage = std::make_shared<LevelTextureStorage>(device, std::make_unique<TextureStorage>(device), level);
            auto mesh_storage = std::make_unique<MeshStorage>(mesh_source, *level, *level_texture_storage);
            auto new_level = std::make_shared<Level>(
                device, 
                shader_storage, 
                level_texture_storage,
                std::make_unique<TransparencyBuffer>(device),
                std::make_unique<SelectionRenderer>(device, shader_storage, std::make_unique<TransparencyBuffer>(device), render_target_source),
                log,
                buffer_source);
            new_level->initialise(
                level,
                std::move(mesh_storage),
                entity_source,
                ai_source,
                room_source,
                trigger_source,
                light_source,
                camera_sink_source);
            return new_level;
        };

        auto dialogs = std::make_shared<Dialogs>(window);
        auto shell = std::make_shared<Shell>();

        auto plugin_source = [=](auto&&... args) { return std::make_shared<Plugin>(files, std::make_unique<Lua>(route_source, randomizer_route_source, waypoint_source, dialogs, files), args...); };
        auto plugins = std::make_shared<Plugins>(
            files,
            std::make_shared<Plugin>(std::make_unique<Lua>(route_source, randomizer_route_source, waypoint_source, dialogs, files), "Default", "trview", "Default Lua plugin for trview"),
            plugin_source,
            settings_loader->load_user_settings());
        auto plugins_window_source = [=]() { return std::make_shared<PluginsWindow>(plugins, shell); };
        auto imgui_backend = std::make_shared<DX11ImGuiBackend>(window, device, files);
        auto fonts = std::make_shared<Fonts>(files, imgui_backend);

        auto viewer_ui = std::make_unique<ViewerUI>(
            window,
            texture_storage,
            shortcuts,
            map_renderer_source,
            std::make_unique<SettingsWindow>(dialogs, shell, fonts),
            std::make_unique<ViewOptions>(),
            std::make_unique<ContextMenu>(),
            std::make_unique<CameraControls>(),
            std::make_unique<Toolbar>(plugins));

        auto clipboard = std::make_shared<Clipboard>(window);

        auto viewer = std::make_unique<Viewer>(
            window,
            device,
            std::move(viewer_ui),
            std::make_unique<Picking>(window),
            std::make_unique<input::Mouse>(window, std::make_unique<input::WindowTester>(window)),
            shortcuts,
            route_source(std::nullopt),
            sprite_source,
            std::make_unique<Compass>(device, sprite_source, render_target_source, mesh_source),
            std::make_unique<Measure>(device, mesh_source),
            render_target_source,
            device_window_source,
            std::make_unique<SectorHighlight>(mesh_source),
            clipboard,
            std::make_unique<Mover>(mesh_source));

        auto items_window_source = [=]() { return std::make_shared<ItemsWindow>(clipboard); };
        auto triggers_window_source = [=]() { return std::make_shared<TriggersWindow>(clipboard); };
        auto route_window_source = [=]() { return std::make_shared<RouteWindow>(clipboard, dialogs, files); };
        auto rooms_window_source = [=]() { return std::make_shared<RoomsWindow>(map_renderer_source, clipboard); };
        auto lights_window_source = [=]() { return std::make_shared<LightsWindow>(clipboard); };

        auto log_window_source = [=]() { return std::make_shared<LogWindow>(log, dialogs, files); };
        auto camera_sink_window_source = [=]() { return std::make_shared<CameraSinkWindow>(clipboard); };

        auto decrypter = std::make_shared<trlevel::Decrypter>();

        auto trlevel_source = [=](auto&& filename) { return std::make_shared<trlevel::Level>(filename, files, decrypter, log); };
        auto textures_window_source = [=]() { return std::make_shared<TexturesWindow>(); };
        auto console_source = [=]() { return std::make_shared<Console>(dialogs, plugins, fonts); };
        auto statics_window_source = [=]() { return std::make_shared<StaticsWindow>(clipboard); };

        return std::make_unique<Application>(
            window,
            std::make_unique<UpdateChecker>(window),
            settings_loader,
            trlevel_source,
            std::make_unique<FileMenu>(window, shortcuts, dialogs, files),
            std::move(viewer),
            route_source,
            shortcuts,
            std::make_unique<ItemsWindowManager>(window, shortcuts, items_window_source),
            std::make_unique<TriggersWindowManager>(window, shortcuts, triggers_window_source),
            std::make_unique<RouteWindowManager>(window, shortcuts, route_window_source),
            std::make_unique<RoomsWindowManager>(window, shortcuts, rooms_window_source),
            level_source,
            std::make_shared<StartupOptions>(command_line),
            dialogs,
            files,
            imgui_backend,
            std::make_unique<LightsWindowManager>(window, shortcuts, lights_window_source),
            std::make_unique<LogWindowManager>(window, log_window_source),
            std::make_unique<TexturesWindowManager>(window, textures_window_source),
            std::make_unique<CameraSinkWindowManager>(window, shortcuts, camera_sink_window_source),
            std::make_unique<ConsoleManager>(window, shortcuts, console_source, files),
            plugins,
            std::make_unique<PluginsWindowManager>(window, shortcuts, plugins_window_source),
            randomizer_route_source,
            fonts,
            std::make_unique<StaticsWindowManager>(window, shortcuts, statics_window_source));
    }
}

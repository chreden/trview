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

#include "Elements/TypeNameLookup.h"
#include "Elements/CameraSink/CameraSink.h"
#include "Elements/Entity.h"
#include "Elements/Light.h"
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
#include "Windows/Textures/TexturesWindowManager.h"
#include "Windows/Textures/TexturesWindow.h"
#include "Windows/CameraSink/CameraSinkWindowManager.h"
#include "Windows/CameraSink/CameraSinkWindow.h"

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
        auto type_name_lookup = std::make_shared<TypeNameLookup>(std::string(type_list.data, type_list.data + type_list.size));

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
        auto waypoint_source = [=](auto&&... args) { return std::make_unique<Waypoint>(waypoint_mesh, args...); };

        auto route_source = [=]()
        {
            return std::make_shared<Route>(
                std::make_unique<SelectionRenderer>(device, shader_storage, std::make_unique<TransparencyBuffer>(device), render_target_source),
                waypoint_source, settings_loader->load_user_settings());
        };

        auto entity_source = [=](auto&& level, auto&& entity, auto&& index, auto&& mesh_storage)
        {
            return std::make_shared<Entity>(mesh_source, level, entity, mesh_storage, index, type_name_lookup->is_pickup(level.get_version(), entity.TypeID));
        };

        auto ai_source = [=](auto&& level, auto&& entity, auto&& index, auto&& mesh_storage)
        {
            return std::make_shared<Entity>(mesh_source, level, entity, mesh_storage, index);
        };

        auto log = std::make_shared<Log>();

        auto bounding_mesh = create_cube_mesh(mesh_source);
        auto static_mesh_source = [=](auto&&... args) { return std::make_shared<StaticMesh>(args..., bounding_mesh); };
        auto static_mesh_position_source = [=](auto&&... args) { return std::make_shared<StaticMesh>(args...); };
        auto sector_source = [=](auto&&... args) { return std::make_shared<Sector>(args...); };
        auto room_source = [=](auto&&... args) { return std::make_shared<Room>( mesh_source, args..., static_mesh_source, static_mesh_position_source, sector_source); };
        auto trigger_source = [=](auto&&... args) { return std::make_shared<Trigger>(args..., mesh_transparent_source); };

        const auto light_mesh = create_sphere_mesh(mesh_source, 24, 24);
        auto light_source = [=](auto&&... args) { return std::make_shared<Light>(light_mesh, args...); };
        auto buffer_source = [=](auto&&... args) { return std::make_unique<graphics::Buffer>(device, args...); };

        auto camera_mesh = create_cube_mesh(mesh_source);
        auto camera_sink_source = [=](auto&&... args) { return std::make_shared<CameraSink>(camera_mesh, texture_storage, args...); };

        auto level_source = [=](auto&& level)
        {
            auto level_texture_storage = std::make_shared<LevelTextureStorage>(device, std::make_unique<TextureStorage>(device), *level);
            auto mesh_storage = std::make_unique<MeshStorage>(mesh_source, *level, *level_texture_storage);
            return std::make_unique<Level>(device, shader_storage, std::move(level),
                level_texture_storage,
                std::move(mesh_storage),
                std::make_unique<TransparencyBuffer>(device),
                std::make_unique<SelectionRenderer>(device, shader_storage, std::make_unique<TransparencyBuffer>(device), render_target_source),
                type_name_lookup,
                entity_source,
                ai_source,
                room_source,
                trigger_source,
                light_source,
                log,
                buffer_source,
                camera_sink_source);
        };

        auto viewer_ui = std::make_unique<ViewerUI>(
            window,
            texture_storage,
            shortcuts,
            map_renderer_source,
            std::make_unique<SettingsWindow>(),
            std::make_unique<ViewOptions>(),
            std::make_unique<ContextMenu>(),
            std::make_unique<CameraControls>());

        auto clipboard = std::make_shared<Clipboard>(window);

        auto viewer = std::make_unique<Viewer>(
            window,
            device,
            std::move(viewer_ui),
            std::make_unique<Picking>(window),
            std::make_unique<input::Mouse>(window, std::make_unique<input::WindowTester>(window)),
            shortcuts,
            route_source(),
            sprite_source,
            std::make_unique<Compass>(device, sprite_source, render_target_source, mesh_source),
            std::make_unique<Measure>(device, mesh_source),
            render_target_source,
            device_window_source,
            std::make_unique<SectorHighlight>(mesh_source),
            clipboard);

        auto dialogs = std::make_shared<Dialogs>(window);

        auto items_window_source = [=]() { return std::make_shared<ItemsWindow>(clipboard); };
        auto triggers_window_source = [=]() { return std::make_shared<TriggersWindow>(clipboard); };
        auto route_window_source = [=]() { return std::make_shared<RouteWindow>(clipboard, dialogs, files); };
        auto rooms_window_source = [=]() { return std::make_shared<RoomsWindow>(map_renderer_source, clipboard); };
        auto lights_window_source = [=]() { return std::make_shared<LightsWindow>(clipboard); };

        auto log_window_source = [=]() { return std::make_shared<LogWindow>(log, dialogs, files); };
        auto camera_sink_window_source = [=]() { return std::make_shared<CameraSinkWindow>(clipboard); };

        auto decrypter = std::make_shared<trlevel::Decrypter>();

        auto trlevel_source = [=](auto&& filename) { return std::make_unique<trlevel::Level>(filename, files, decrypter, log); };
        auto textures_window_source = [=]() { return std::make_shared<TexturesWindow>(); };

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
            std::make_unique<DX11ImGuiBackend>(window, device),
            std::make_unique<LightsWindowManager>(window, shortcuts, lights_window_source),
            std::make_unique<LogWindowManager>(window, log_window_source),
            std::make_unique<TexturesWindowManager>(window, textures_window_source),
            std::make_unique<CameraSinkWindowManager>(window, shortcuts, camera_sink_window_source));
    }
}

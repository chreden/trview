#include "Application.h"

#include <trlevel/LevelLoader.h>
#include <trview.common/Files.h>
#include <trview.common/windows/Clipboard.h>
#include <trview.common/Windows/Dialogs.h>
#include <trview.common/Windows/Shell.h>

#include <trview.graphics/ShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.graphics/DeviceWindow.h>
#include <trview.graphics/RenderTarget.h>
#include <trview.input/WindowTester.h>

#include "Resources/resource.h"
#include "Resources/DefaultShaders.h"
#include "Resources/DefaultTextures.h"
#include "Resources/DefaultFonts.h"

#include "Elements/TypeNameLookup.h"
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
#include "Menus/FileDropper.h"
#include "Menus/LevelSwitcher.h"
#include "Menus/RecentFiles.h"
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
#include "UI/DX11ImGuiBackend.h"

namespace trview
{
    namespace
    {
        template <typename T>
        std::shared_ptr<T> create_shared(auto&&... args)
        {
            return std::make_shared<T>(args...);
        }
    }

    std::unique_ptr<IApplication> create_application(const Window& window, const std::wstring& command_line)
    {
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
                waypoint_source);
        };

        auto entity_source = [=](auto&& level, auto&& entity, auto&& index, auto&& mesh_storage)
        {
            return std::make_shared<Entity>(mesh_source, level, entity, mesh_storage, index, type_name_lookup->is_pickup(level.get_version(), entity.TypeID));
        };

        auto ai_source = [=](auto&& level, auto&& entity, auto&& index, auto&& mesh_storage)
        {
            return std::make_shared<Entity>(mesh_source, level, entity, mesh_storage, index);
        };

        auto bounding_mesh = create_cube_mesh(mesh_source);
        auto static_mesh_source = [=](auto&&... args) { return std::make_shared<StaticMesh>(args..., bounding_mesh); };
        auto static_mesh_position_source = [=](auto&&... args) { return std::make_shared<StaticMesh>(args...); };
        auto sector_source = [=](auto&&... args) { return std::make_shared<Sector>(args...); };
        auto room_source = [=](auto&&... args) { return std::make_shared<Room>( mesh_source, args..., static_mesh_source, static_mesh_position_source, sector_source); };
        auto trigger_source = [=](auto&&... args) { return std::make_shared<Trigger>(args..., mesh_transparent_source); };

        const auto light_mesh = create_sphere_mesh(mesh_source, 24, 24);
        auto light_source = [=](auto&&... args) { return std::make_shared<Light>(light_mesh, args...); };

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
                light_source);
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
            std::make_unique<SectorHighlight>(mesh_source));

        auto files = std::make_shared<Files>();
        auto dialogs = std::make_shared<Dialogs>(window);
        auto clipboard = std::make_shared<Clipboard>(window);

        auto items_window_source = [=]() { return std::make_shared<ItemsWindow>(clipboard); };
        auto triggers_window_source = [=]() { return std::make_shared<TriggersWindow>(clipboard); };
        auto route_window_source = [=]() { return std::make_shared<RouteWindow>(clipboard, dialogs, files); };
        auto rooms_window_source = [=]() { return std::make_shared<RoomsWindow>(map_renderer_source, clipboard); };
        auto lights_window_source = [=]() { return std::make_shared<LightsWindow>(clipboard); };

        return std::make_unique<Application>(
            window,
            std::make_unique<UpdateChecker>(window),
            std::make_unique<SettingsLoader>(files),
            std::make_unique<FileDropper>(window),
            std::make_unique<trlevel::LevelLoader>(),
            std::make_unique<LevelSwitcher>(window),
            std::make_unique<RecentFiles>(window),
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
            std::make_unique<LightsWindowManager>(window, shortcuts, lights_window_source));
    }
}

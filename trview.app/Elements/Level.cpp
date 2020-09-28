#define NOMINMAX
#include "Level.h"

#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IShader.h>

#include <trview.app/Graphics/LevelTextureStorage.h>
#include <trview.app/Graphics/IMeshStorage.h>
#include <trview.app/Camera/ICamera.h>
#include <trview.app/Geometry/TransparencyBuffer.h>
#include <trview.app/Graphics/SelectionRenderer.h>
#include <trview.app/Graphics/MeshStorage.h>
#include <trview.app/Elements/ITypeNameLookup.h>

#include <trview.lau/drm.h>
#include <trview.common/Strings.h>

#include <external/DirectXTK/Inc/DDSTextureLoader.h>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    Level::Level(const graphics::Device& device, const graphics::IShaderStorage& shader_storage, std::unique_ptr<trlevel::ILevel>&& level, const ITypeNameLookup& type_names)
        : _version(level->get_version())
    {
        _vertex_shader = shader_storage.get("level_vertex_shader");
        _pixel_shader = shader_storage.get("level_pixel_shader");

        // Create a texture sampler state description.
        D3D11_SAMPLER_DESC sampler_desc;
        memset(&sampler_desc, 0, sizeof(sampler_desc));
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.MaxAnisotropy = 1;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

        // Create the texture sampler state.
        device.device()->CreateSamplerState(&sampler_desc, &_sampler_state);

        _texture_storage = std::make_unique<LevelTextureStorage>(device, *level);
        _mesh_storage = std::make_unique<MeshStorage>(device, *level, *_texture_storage.get());
        generate_rooms(device, *level);
        generate_triggers();
        generate_entities(device, *level, type_names);

        for (auto& room : _rooms)
        {
            room->update_bounding_box();
        }

        _transparency = std::make_unique<TransparencyBuffer>(device);

        _selection_renderer = std::make_unique<SelectionRenderer>(device, shader_storage);
    }

    Level::~Level()
    {
    }

    std::vector<RoomInfo> Level::room_info() const
    {
        std::vector<RoomInfo> room_infos;
        for (const auto& r : _rooms)
        {
            room_infos.push_back(r->info());
        }
        return room_infos;
    }

    RoomInfo Level::room_info(uint32_t room) const
    {
        return _rooms[room]->info();
    }

    std::vector<graphics::Texture> Level::level_textures() const
    {
        std::vector<graphics::Texture> textures;
        for (uint32_t i = 0; i < _texture_storage->num_tiles(); ++i)
        {
            textures.push_back(_texture_storage->texture(i));
        }
        return textures;
    }

    uint16_t Level::selected_room() const
    {
        return _selected_room;
    }

    const std::vector<Item>& Level::items() const
    {
        return _items;
    }

    uint32_t Level::number_of_rooms() const
    {
        return _rooms.size();
    }

    std::vector<Room*> Level::rooms() const
    {
        std::vector<Room*> rooms;
        std::transform(_rooms.begin(), _rooms.end(), std::back_inserter(rooms), [](const auto& room) { return room.get(); });
        return rooms;
    }

    std::vector<Trigger*> Level::triggers() const
    {
        std::vector<Trigger*> triggers;
        std::transform(_triggers.begin(), _triggers.end(), std::back_inserter(triggers),
            [](const auto& trigger) { return trigger.get(); });
        return triggers;
    }

    void Level::set_highlight_mode(RoomHighlightMode mode, bool enabled)
    {
        if (enabled)
        {
            _room_highlight_modes.insert(mode);
        }
        else
        {
            _room_highlight_modes.erase(mode);
        }

        regenerate_neighbours();
        _regenerate_transparency = true;
        on_level_changed();
    }

    bool Level::highlight_mode_enabled(RoomHighlightMode mode) const
    {
        return _room_highlight_modes.find(mode) != _room_highlight_modes.end();
    }

    void Level::set_selected_room(uint16_t index)
    { 
        _selected_room = index;
        regenerate_neighbours();

        // If the user has selected a room that is or has an alternate mode, raise the event that the
        // alternate mode needs to change so that the correct rooms can be rendered.
        const auto& room = *_rooms[index];
        if (is_alternate_mismatch(room))
        {
            if (version() >= trlevel::LevelVersion::Tomb4)
            {
                on_alternate_group_selected(room.alternate_group(), !is_alternate_group_set(room.alternate_group()));
            }
            else
            {
                on_alternate_mode_selected(!_alternate_mode);
            }
        }

        on_level_changed();
    }

    void Level::set_selected_item(uint32_t index)
    {
        _selected_item = _entities[index].get();
        on_level_changed();
    }

    void Level::set_neighbour_depth(uint32_t depth)
    {
        _neighbour_depth = depth;
        regenerate_neighbours();
        on_level_changed();
    }

    void Level::render(const graphics::Device& device, const ICamera& camera, bool render_selection)
    {
        using namespace DirectX;

        auto context = device.context();

        context->PSSetSamplers(0, 1, &_sampler_state);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _vertex_shader->apply(context);
        _pixel_shader->apply(context);

        render_rooms(device, camera);
        if (render_selection)
        {
            render_selected_item(device, camera);
        }
    }

    namespace
    {
        struct DDS_PIXELFORMAT {
            DWORD dwSize;
            DWORD dwFlags;
            DWORD dwFourCC;
            DWORD dwRGBBitCount;
            DWORD dwRBitMask;
            DWORD dwGBitMask;
            DWORD dwBBitMask;
            DWORD dwABitMask;
        };

        typedef struct {
            DWORD           dwSize;
            DWORD           dwFlags;
            DWORD           dwHeight;
            DWORD           dwWidth;
            DWORD           dwPitchOrLinearSize;
            DWORD           dwDepth;
            DWORD           dwMipMapCount;
            DWORD           dwReserved1[11];
            DDS_PIXELFORMAT ddspf;
            DWORD           dwCaps;
            DWORD           dwCaps2;
            DWORD           dwCaps3;
            DWORD           dwCaps4;
            DWORD           dwReserved2;
        } DDS_HEADER;
    }

    // Render the rooms in the level.
    // context: The device context.
    // camera: The current camera to render the level with.
    void Level::render_rooms(const graphics::Device& device, const ICamera& camera)
    {
        // Only render the rooms that the current view mode includes.
        auto rooms = get_rooms_to_render(camera);

        if (_regenerate_transparency)
        {
            _transparency->reset();
        }

#if 1
        static std::vector<TransparentTriangle> tris;
        if (tris.empty())
        {
            auto drm = lau::load_drm(std::wstring(L"C:\\Projects\\Applications\\trview\\lau\\drm\\") + to_utf16("ma9") + L".drm");

            std::vector<ComPtr<ID3D11Resource>> textures;
            for (auto& t : drm->textures)
            {
                auto& data = t.second;
                ComPtr<ID3D11Resource> resource;
                ComPtr<ID3D11ShaderResourceView> resource_view;

                DDS_HEADER header;
                memset(&header, 0, sizeof(header));
                header.dwSize = sizeof(header);
                header.dwFlags = 0x1 | 0x2 | 0x4 | 0x1000 | 0x80000;
                header.dwHeight = data.height;
                header.dwWidth = data.width;
                header.dwPitchOrLinearSize = data.data.size();
                header.ddspf.dwSize = sizeof(header.ddspf); 
                // header.ddspf.dwFlags = 0x1; // Alpha?
                header.ddspf.dwFlags = 4;
                header.ddspf.dwFourCC = 827611204;
                header.dwCaps = 4096;

                std::vector<uint8_t> final_data(4 + sizeof(header) + data.data.size(), 0u);
                uint32_t magic = 542327876u;
                memcpy(&final_data[0], &magic, sizeof(magic));
                memcpy(&final_data[4], &header, sizeof(header));
                memcpy(&final_data[4 + sizeof(header)], &data.data[0], data.data.size());

                std::ofstream outer;
                outer.open(std::wstring(L"C:\\Users\\Chris\\AppData\\Local\\Temp\\Temper\\xepeeaqi\\test") + std::to_wstring(t.first) + L".dds", std::ios::binary | std::ios::out);
                outer.write(reinterpret_cast<char*>(&final_data[0]), final_data.size());
                outer.close();

                HRESULT hr = DirectX::CreateDDSTextureFromMemory(device.device().Get(), &final_data[0], final_data.size(), resource.GetAddressOf(), resource_view.GetAddressOf());
                textures.push_back(resource);
            }

            auto m = Matrix::CreateScale(1.0 / 2048.0f);

            for (auto& tri : drm->world_triangles)
            {
                auto v0 = Vector3(drm->world_mesh[tri.v0].x, drm->world_mesh[tri.v0].y, drm->world_mesh[tri.v0].z);
                auto v1 = Vector3(drm->world_mesh[tri.v1].x, drm->world_mesh[tri.v1].y, drm->world_mesh[tri.v1].z);
                auto v2 = Vector3(drm->world_mesh[tri.v2].x, drm->world_mesh[tri.v2].y, drm->world_mesh[tri.v2].z);
                auto value = std::min(0.2f, rand() / static_cast<float>(RAND_MAX));
                tris.push_back(TransparentTriangle(v0, v2, v1, Color(1, 1, 1, 1)).transform(m, Color(value, value, value, 1)));
            }
        }

        for (const auto& t : tris)
        {
            _transparency->add(t);
        }

#else

        // Render the opaque portions of the rooms and also collect the transparent triangles
        // that need to be rendered in the second pass.
        for (const auto& room : rooms)
        {
            room.room.render(device, camera, *_texture_storage.get(), room.selection_mode, _show_hidden_geometry, _show_water);
            if (_regenerate_transparency)
            {
                room.room.get_transparent_triangles(*_transparency, camera, room.selection_mode, _show_triggers, _show_water);
            }

            // If this is an alternate room, render the items from the original room in the sample places.
            if (!is_alternate_mismatch(room.room) && room.room.alternate_mode() == Room::AlternateMode::IsAlternate)
            {
                auto& original_room = _rooms[room.room.alternate_room()];
                original_room->render_contained(device, camera, *_texture_storage.get(), room.selection_mode, room.room.water(), _show_water);
                if (_regenerate_transparency)
                {
                    original_room->get_contained_transparent_triangles(*_transparency, camera, room.selection_mode, room.room.water(), _show_water);
                }
            }
        }
        
#endif 
        if (_regenerate_transparency)
        {
            // Sort the accumulated transparent triangles farthest to nearest.
            _transparency->sort(camera.rendering_position());
        }

        _regenerate_transparency = false;
    }

    void Level::render_transparency(const graphics::Device& device, const ICamera& camera)
    {
        // Render the triangles that the transparency buffer has produced.
        _transparency->render(device.context(), camera, *_texture_storage.get());
    }

    void Level::render_selected_item(const graphics::Device& device, const ICamera& camera)
    {
        const Color Trigger_Outline{ 0.0f, 1.0f, 0.0f, 1.0f };
        const Color Item_Outline{ 1.0f, 1.0f, 0.0f, 1.0f };

        if (_selected_item)
        {
            _selection_renderer->render(device, camera, *_texture_storage, *_selected_item, Item_Outline);
        }

        if (_selected_trigger)
        {
            _selection_renderer->render(device, camera, *_texture_storage, *_selected_trigger, Trigger_Outline);
        }
    }

    // Get the collection of rooms that need to be renderered depending on the current view mode.
    // Returns: The rooms to render and their selection mode.
    std::vector<Level::RoomToRender> Level::get_rooms_to_render(const ICamera& camera) const
    {
        std::vector<RoomToRender> rooms;

        DirectX::BoundingFrustum frustum = camera.frustum();

        auto in_view = [&](const Room& room)
        {
            return camera.projection_mode() == ProjectionMode::Orthographic || frustum.Contains(room.bounding_box()) != DirectX::DISJOINT;
        };
    
        bool highlight = highlight_mode_enabled(RoomHighlightMode::Highlight);
        if (highlight_mode_enabled(RoomHighlightMode::Neighbours))
        {
            for (uint16_t i : _neighbours)
            {
                const auto& room = _rooms[i];
                if (is_alternate_mismatch(*room) || !in_view(*room))
                {
                    continue;
                }
                rooms.emplace_back(*room.get(), highlight ? (i == _selected_room ? Room::SelectionMode::Selected : Room::SelectionMode::NotSelected) : Room::SelectionMode::Selected, i);
            }
        }
        else
        {
            for (std::size_t i = 0; i < _rooms.size(); ++i)
            {
                const auto& room = _rooms[i].get();
                if (is_alternate_mismatch(*room) || !in_view(*room))
                {
                    continue;
                }
                rooms.emplace_back(*room, highlight ? (_selected_room == static_cast<uint16_t>(i) ? Room::SelectionMode::Selected : Room::SelectionMode::NotSelected) : Room::SelectionMode::Selected, static_cast<uint16_t>(i));
            }
        }

        return rooms;
    }

    void Level::generate_rooms(const graphics::Device& device, const trlevel::ILevel& level)
    {
        const auto num_rooms = level.num_rooms();
        for (uint32_t i = 0u; i < num_rooms; ++i)
        {
            auto room = level.get_room(i);
            _rooms.push_back(std::make_unique<Room>(device, level, room, *_texture_storage.get(), *_mesh_storage.get(), i, *this));
        }

        std::set<uint32_t> alternate_groups;

        // Fix up the IsAlternate status of the rooms that are referenced by HasAlternate rooms.
        // This can only be done once all the rooms are loaded.
        for (auto i = 0u; i < _rooms.size(); ++i)
        {
            const auto& room = _rooms[i];
            if (room->alternate_mode() == Room::AlternateMode::HasAlternate)
            {
                alternate_groups.insert(room->alternate_group());

                int16_t alternate = room->alternate_room();
                if (alternate != -1)
                {
                    _rooms[alternate]->set_is_alternate(static_cast<int16_t>(i));
                }
            }
        }
    }

    void Level::generate_triggers()
    {
        for (auto i = 0u; i < _rooms.size(); ++i)
        {
            const auto& room = _rooms[i];
            for (auto sector : room->sectors())
            {
                if (sector->flags & SectorFlag::Trigger)
                {
                    _triggers.emplace_back(std::make_unique<Trigger>(_triggers.size(), i, sector->x(), sector->z(), sector->trigger()));
                    room->add_trigger(_triggers.back().get());
                }
            }
        }

        for (auto& room : _rooms)
        {
            room->generate_trigger_geometry();
        }
    }

    void Level::generate_entities(const graphics::Device& device, const trlevel::ILevel& level, const ITypeNameLookup& type_names)
    {
        const uint32_t num_entities = level.num_entities();
        for (uint32_t i = 0; i < num_entities; ++i)
        {
            // Entity for rendering.
            auto level_entity = level.get_entity(i);
            auto entity = std::make_unique<Entity>(device, level, level_entity, *_texture_storage.get(), *_mesh_storage.get(), i);
            _rooms[entity->room()]->add_entity(entity.get());
            _entities.push_back(std::move(entity));

            // Relevant triggers.
            std::vector<Trigger*> relevant_triggers;
            for (const auto& trigger : _triggers)
            {
                if (trigger->triggers_item(i))
                {
                    relevant_triggers.push_back(trigger.get());
                }
            }

            // Item for item information.
            _items.emplace_back(i, level_entity.Room, level_entity.TypeID, type_names.lookup_type_name(_version, level_entity.TypeID), version() >= trlevel::LevelVersion::Tomb4 ? level_entity.Intensity2 : 0, level_entity.Flags, relevant_triggers, level_entity.position());
        }
    }

    void Level::regenerate_neighbours()
    {
        _neighbours.clear();
        if (_selected_room < number_of_rooms())
        {
            generate_neighbours(_neighbours, _selected_room, _neighbour_depth);
            _regenerate_transparency = true;
        }
    }

    void Level::generate_neighbours(std::set<uint16_t>& results, uint16_t selected_room, int32_t max_depth)
    {
        results.insert(selected_room);

        std::set<uint16_t> current_batch{ selected_room };
        std::set<uint16_t> next_batch;

        for (int32_t depth = 0; depth <= max_depth && !current_batch.empty(); ++depth)
        {
            for (uint16_t room : current_batch)
            {
                // Add the room to the final list of rooms.
                results.insert(room);

                // Get the neighbours of the room and add them to the next batch to be processed.
                const auto neighbours = _rooms[room]->neighbours();
                next_batch.insert(neighbours.begin(), neighbours.end());
            }

            // Remove all entries from the current batch as they have been processed.
            current_batch.clear();

            // Add any entries that aren't already in the results to the list to be processed.
            for (uint16_t room : next_batch)
            {
                if (results.find(room) == results.end())
                {
                    current_batch.insert(room);
                }
            }

            // Clear the next batch.
            next_batch.clear();
        }
    }

    // Determine whether the specified ray hits any of the triangles in any of the room geometry.
    // position: The world space position of the source of the ray.
    // direction: The direction of the ray.
    // Returns: The result of the operation. If 'hit' is true, distance and position contain
    // how far along the ray the hit was and the position in world space. The room that was hit
    // is also specified.
    PickResult Level::pick(const ICamera& camera, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        PickResult final_result;
        
        auto choose = [&](PickResult result)
        {
            // Choose the nearest pick - but if the previous closest was trigger an entity should take priority over it.
            if (result.hit && (result.distance < final_result.distance || (result.type == PickResult::Type::Entity && final_result.type == PickResult::Type::Trigger)))
            {
                final_result.hit = true;
                final_result.distance = result.distance;
                final_result.position = result.position;
                final_result.index = result.index;
                final_result.type = result.type;
            }
        };

        auto rooms = get_rooms_to_render(camera);
        for (auto& room : rooms)
        {
            choose(room.room.pick(position, direction, true, _show_triggers, _show_hidden_geometry));
            if (!is_alternate_mismatch(room.room) && room.room.alternate_mode() == Room::AlternateMode::IsAlternate)
            {
                auto& original_room = _rooms[room.room.alternate_room()];
                choose(original_room->pick(position, direction, true, false, false, false));
            }
        }
        return final_result;
    }

    // Determines whether the room is currently being rendered.
    // room: The room index.
    // Returns: True if the room is visible.
    bool Level::room_visible(uint32_t room) const
    {
        if (highlight_mode_enabled(RoomHighlightMode::Neighbours))
        {
            return true;
        }
        return _neighbours.find(static_cast<uint16_t>(room)) != _neighbours.end();
    }

    void Level::on_camera_moved()
    {
        _regenerate_transparency = true;
    }

    // Set whether to render the alternate mode (the flipmap) or the regular room.
    // enabled: Whether to render the flipmap.
    void Level::set_alternate_mode(bool enabled)
    {
        _alternate_mode = enabled;
        _regenerate_transparency = true;

        // If the currently selected room is a room involved in flipmaps, select the alternate
        // room so that the user doesn't have an invisible room selected.
        const auto& current_room = *_rooms[selected_room()];
        if (is_alternate_mismatch(current_room))
        {
            on_room_selected(current_room.alternate_room());
        }

        on_level_changed();
    }

    void Level::set_alternate_group(uint32_t group, bool enabled)
    {
        _regenerate_transparency = true;
        if (enabled)
        {
            _alternate_groups.insert(group);
        }
        else
        {
            _alternate_groups.erase(group);
        }

        // If the currently selected room is a room involved in flipmaps, select the alternate
        // room so that the user doesn't have an invisible room selected.
        const auto& current_room = *_rooms[selected_room()];
        if (is_alternate_mismatch(current_room))
        {
            on_room_selected(current_room.alternate_room());
        }

        on_level_changed();
    }

    bool Level::alternate_group(uint32_t group) const
    {
        return _alternate_groups.find(group) != _alternate_groups.end();
    }

    // Determines whether the alternate mode specified is a mismatch with the current setting of 
    // the alternate mode flag.
    bool Level::is_alternate_mismatch(const Room& room) const
    {
        if (version() >= trlevel::LevelVersion::Tomb4)
        {
            return room.alternate_mode() == Room::AlternateMode::HasAlternate && is_alternate_group_set(room.alternate_group()) ||
                   room.alternate_mode() == Room::AlternateMode::IsAlternate && !is_alternate_group_set(room.alternate_group());
        }

        return room.alternate_mode() == Room::AlternateMode::IsAlternate && !_alternate_mode ||
               room.alternate_mode() == Room::AlternateMode::HasAlternate && _alternate_mode;
    }

    // Get the current state of the alternate mode (flipmap).
    bool Level::alternate_mode() const
    {
        return _alternate_mode;
    }

    bool Level::any_alternates() const
    {
        return std::any_of(_rooms.begin(), _rooms.end(), [](const std::unique_ptr<Room>& room)
        {
            return room->alternate_mode() != Room::AlternateMode::None;
        });
    }

    void Level::set_show_triggers(bool show)
    {
        _show_triggers = show;
        _regenerate_transparency = true;
        on_level_changed();
    }

    void Level::set_show_hidden_geometry(bool show)
    {
        _show_hidden_geometry = show;
        on_level_changed();
    }

    bool Level::show_hidden_geometry() const
    {
        return _show_hidden_geometry;
    }

    void Level::set_show_water(bool show)
    {
        _show_water = show;
        _regenerate_transparency = true;
        on_level_changed();
    }

    bool Level::show_triggers() const
    {
        return _show_triggers;
    }

    void Level::set_selected_trigger(uint32_t number)
    {
        _selected_trigger = _triggers[number].get();
        on_level_changed();
    }

    const ILevelTextureStorage& Level::texture_storage() const
    {
        return *_texture_storage;
    }

    std::set<uint32_t> Level::alternate_groups() const
    {
        std::set<uint32_t> groups;
        for (auto i = 0u; i < _rooms.size(); ++i)
        {
            const auto& room = _rooms[i];
            if (room->alternate_mode() != Room::AlternateMode::None)
            {
                groups.insert(room->alternate_group());
            }
        }
        return groups;
    }

    bool Level::is_alternate_group_set(uint16_t group) const
    {
        return _alternate_groups.find(group) != _alternate_groups.end();
    }

    trlevel::LevelVersion Level::version() const
    {
        return _version;
    }

    bool find_item_by_type_id(const Level& level, uint32_t type_id, Item& output_item)
    {
        const auto& items = level.items();
        auto found_item = std::find_if(items.begin(), items.end(), [=](const auto& item) { return item.type_id() == type_id; });
        if (found_item == items.end())
        {
            return false;
        }
        output_item = *found_item;
        return true;
    }
}

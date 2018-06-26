#include "stdafx.h"
#include "Level.h"

#include <trview.common/FileLoader.h>

#include "LevelTextureStorage.h"
#include "MeshStorage.h"

#include "ICamera.h"
#include "TransparencyBuffer.h"

#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IShader.h>

using namespace Microsoft::WRL;

namespace trview
{
    Level::Level(const ComPtr<ID3D11Device>& device, const graphics::IShaderStorage& shader_storage, const trlevel::ILevel* level)
        : _level(level)
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
        device->CreateSamplerState(&sampler_desc, &_sampler_state);

        _texture_storage = std::make_unique<LevelTextureStorage>(device, *_level);
        _mesh_storage = std::make_unique<MeshStorage>(device, *_level, *_texture_storage.get());
        generate_rooms(device);
        generate_entities(device);

        _transparency = std::make_unique<TransparencyBuffer>(device);
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

    Level::RoomHighlightMode Level::highlight_mode() const
    {
        return _room_highlight_mode;
    }

    void Level::set_highlight_mode(RoomHighlightMode mode)
    {
        if (_room_highlight_mode == mode)
        {
            return;
        }

        _room_highlight_mode = mode;
        if (_room_highlight_mode == RoomHighlightMode::Neighbours)
        {
            regenerate_neighbours();
        }
        _regenerate_transparency = true;
    }

    void Level::set_selected_room(uint16_t index)
    { 
        _selected_room = index;
        regenerate_neighbours();

        // If the user has selected a room that is or has an alternate mode, raise the event that the
        // alternate mode needs to change so that the correct rooms can be rendered.
        const auto& room = *_rooms[index];
        if (is_alternate_mismatch(room.alternate_mode()))
        {
            on_alternate_mode_selected(!_alternate_mode);
        }
    }

    void Level::set_neighbour_depth(uint32_t depth)
    {
        _neighbour_depth = depth;
        regenerate_neighbours();
    }

    void Level::render(const ComPtr<ID3D11DeviceContext>& context, const ICamera& camera)
    {
        using namespace DirectX;

        context->PSSetSamplers(0, 1, &_sampler_state);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        _vertex_shader->apply(context);
        _pixel_shader->apply(context);

        render_rooms(context, camera);
    }

    // Render the rooms in the level.
    // context: The device context.
    // camera: The current camera to render the level with.
    void Level::render_rooms(const ComPtr<ID3D11DeviceContext>& context, const ICamera& camera)
    {
        // Only render the rooms that the current view mode includes.
        auto rooms = get_rooms_to_render();

        if (_regenerate_transparency)
        {
            _transparency->reset();
        }

        // Render the opaque portions of the rooms and also collect the transparent triangles
        // that need to be rendered in the second pass.
        for (const auto& room : rooms)
        {
            room.room.render(context, camera, *_texture_storage.get(), room.selection_mode);
            if (_regenerate_transparency)
            {
                room.room.get_transparent_triangles(*_transparency, camera, room.selection_mode);
            }

            // If this is an alternate room, render the items from the original room in the sample places.
            if (_alternate_mode && room.room.alternate_mode() == Room::AlternateMode::IsAlternate)
            {
                auto& original_room = _rooms[room.room.alternate_room()];
                original_room->render_contained(context, camera, *_texture_storage.get(), room.selection_mode);
                if (_regenerate_transparency)
                {
                    original_room->get_contained_transparent_triangles(*_transparency, camera, room.selection_mode);
                }
            }
        }

        if (_regenerate_transparency)
        {
            // Sort the accumulated transparent triangles farthest to nearest.
            _transparency->sort(camera.position());
        }

        _regenerate_transparency = false;

        // Render the triangles that the transparency buffer has produced.
        _transparency->render(context, camera, *_texture_storage.get());
    }

    // Get the collection of rooms that need to be renderered depending on the current view mode.
    // Returns: The rooms to render and their selection mode.
    std::vector<Level::RoomToRender> Level::get_rooms_to_render() const
    {
        std::vector<RoomToRender> rooms;
        switch (_room_highlight_mode)
        {
            case RoomHighlightMode::None:
            {
                for (std::size_t i = 0; i < _rooms.size(); ++i)
                {
                    const auto& room = _rooms[i].get();
                    if (is_alternate_mismatch(room->alternate_mode()))
                    {
                        continue;
                    }
                    rooms.emplace_back(*room, Room::SelectionMode::Selected, static_cast<uint16_t>(i));
                }
                break;
            }
            case RoomHighlightMode::Highlight:
            {
                for (std::size_t i = 0; i < _rooms.size(); ++i)
                {
                    const auto& room = _rooms[i];
                    if (is_alternate_mismatch(room->alternate_mode()))
                    {
                        continue;
                    }
                    rooms.emplace_back(*room.get(), _selected_room == static_cast<uint16_t>(i) ? Room::SelectionMode::Selected : Room::SelectionMode::NotSelected, static_cast<uint16_t>(i));
                }
                break;
            }
            case RoomHighlightMode::Neighbours:
            {
                for (uint16_t i : _neighbours)
                {
                    const auto& room = _rooms[i];
                    if (is_alternate_mismatch(room->alternate_mode()))
                    {
                        continue;
                    }
                    rooms.emplace_back(*room.get(), i == _selected_room ? Room::SelectionMode::Selected : Room::SelectionMode::Neighbour, i);
                }
                break;
            }
        }
        return rooms;
    }

    void Level::generate_rooms(const Microsoft::WRL::ComPtr<ID3D11Device>& device)
    {
        const uint16_t num_rooms = _level->num_rooms();
        for (uint16_t i = 0; i < num_rooms; ++i)
        {
            auto room = _level->get_room(i);
            _rooms.push_back(std::make_unique<Room>(device, *_level, room, *_texture_storage.get(), *_mesh_storage.get()));
        }

        // Fix up the IsAlternate status of the rooms that are referenced by HasAlternate rooms.
        // This can only be done once all the rooms are loaded.
        for (int16_t i = 0; i < _rooms.size(); ++i)
        {
            const auto& room = _rooms[i];
            if (room->alternate_mode() == Room::AlternateMode::HasAlternate)
            {
                int16_t alternate = room->alternate_room();
                if (alternate != -1)
                {
                    _rooms[alternate]->set_is_alternate(i);
                }
            }
        }
    }

    void Level::generate_entities(const ComPtr<ID3D11Device>& device)
    {
        const uint32_t num_entities = _level->num_entities();
        for (uint32_t i = 0; i < num_entities; ++i)
        {
            auto level_entity = _level->get_entity(i);
            auto entity = std::make_unique<Entity>(device, *_level, level_entity, *_texture_storage.get(), *_mesh_storage.get());
            _rooms[entity->room()]->add_entity(entity.get());
            _entities.push_back(std::move(entity));
        }
    }

    void Level::regenerate_neighbours()
    {
        _neighbours = std::set<uint16_t>{ _selected_room };
        if (_selected_room < _level->num_rooms())
        {
            generate_neighbours(_neighbours, _selected_room, _selected_room, 1, _neighbour_depth);
            _regenerate_transparency = true;
        }
    }

    void Level::generate_neighbours(std::set<uint16_t>& all_rooms, uint16_t previous_room, uint16_t selected_room, int32_t current_depth, int32_t max_depth)
    {
        if (current_depth > max_depth)
        {
            return;
        }

        const auto neighbours = _rooms[selected_room]->neighbours();
        for (auto room = neighbours.begin(); room != neighbours.end(); ++room)
        {
            if (*room != previous_room)
            {
                all_rooms.insert(*room);
                generate_neighbours(all_rooms, selected_room, *room, current_depth + 1, max_depth);
            }
        }
    }

    // Determine whether the specified ray hits any of the triangles in any of the room geometry.
    // position: The world space position of the source of the ray.
    // direction: The direction of the ray.
    // Returns: The result of the operation. If 'hit' is true, distance and position contain
    // how far along the ray the hit was and the position in world space. The room that was hit
    // is also specified.
    Level::PickResult Level::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        PickResult final_result;
        auto rooms = get_rooms_to_render();
        for (auto& room : rooms)
        {
            auto result = room.room.pick(position, direction);
            if (result.hit && result.distance < final_result.distance)
            {
                final_result.hit = true;
                final_result.distance = result.distance;
                final_result.position = result.position;
                final_result.room = room.number;
            }
        }
        return final_result;
    }

    // Determines whether the room is currently being rendered.
    // room: The room index.
    // Returns: True if the room is visible.
    bool Level::room_visible(uint32_t room) const
    {
        if (_room_highlight_mode != RoomHighlightMode::Neighbours)
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
        if (is_alternate_mismatch(current_room.alternate_mode()))
        {
            on_room_selected(current_room.alternate_room());
        }
    }

    // Determines whether the alternate mode specified is a mismatch with the current setting of 
    // the alternate mode flag.
    bool Level::is_alternate_mismatch(Room::AlternateMode mode) const
    {
        return mode == Room::AlternateMode::IsAlternate && !_alternate_mode ||
               mode == Room::AlternateMode::HasAlternate && _alternate_mode;
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
}

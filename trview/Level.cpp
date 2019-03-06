#include "stdafx.h"
#include "Level.h"

#include <sstream>

#include <trview.common/FileLoader.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IShader.h>

#include "LevelTextureStorage.h"
#include "MeshStorage.h"
#include <trview.app/ICamera.h>
#include <trview.app/TransparencyBuffer.h>
#include "ResourceHelper.h"
#include "resource.h"
#include <trview.app/SelectionRenderer.h>

#include <external/nlohmann/json.hpp>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    Level::Level(const graphics::Device& device, const graphics::IShaderStorage& shader_storage, const trlevel::ILevel* level)
        : _level(level)
    {
        load_type_name_lookup();

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

        _texture_storage = std::make_unique<LevelTextureStorage>(device, *_level);
        _mesh_storage = std::make_unique<MeshStorage>(device, *_level, *_texture_storage.get());
        generate_rooms(device);
        generate_triggers();
        generate_entities(device);

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
            if (_level->get_version() >= trlevel::LevelVersion::Tomb4)
            {
                on_alternate_group_selected(room.alternate_group(), !is_alternate_group_set(room.alternate_group()));
            }
            else
            {
                on_alternate_mode_selected(!_alternate_mode);
            }
        }
    }

    void Level::set_selected_item(uint16_t index)
    {
        _selected_item = _entities[index].get();
    }

    void Level::set_neighbour_depth(uint32_t depth)
    {
        _neighbour_depth = depth;
        regenerate_neighbours();
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
                original_room->render_contained(device, camera, *_texture_storage.get(), room.selection_mode, _show_water, _show_water);
                if (_regenerate_transparency)
                {
                    original_room->get_contained_transparent_triangles(*_transparency, camera, room.selection_mode, _show_water, _show_water);
                }
            }
        }

        if (_regenerate_transparency)
        {
            // Sort the accumulated transparent triangles farthest to nearest.
            _transparency->sort(camera.position());
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
            return frustum.Contains(room.bounding_box()) != DirectX::DISJOINT;
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

    void Level::generate_rooms(const graphics::Device& device)
    {
        const uint16_t num_rooms = _level->num_rooms();
        for (uint16_t i = 0; i < num_rooms; ++i)
        {
            auto room = _level->get_room(i);
            _rooms.push_back(std::make_unique<Room>(device, *_level, room, *_texture_storage.get(), *_mesh_storage.get(), i, *this));
        }

        std::set<uint32_t> alternate_groups;

        // Fix up the IsAlternate status of the rooms that are referenced by HasAlternate rooms.
        // This can only be done once all the rooms are loaded.
        for (int16_t i = 0; i < _rooms.size(); ++i)
        {
            const auto& room = _rooms[i];
            if (room->alternate_mode() == Room::AlternateMode::HasAlternate)
            {
                alternate_groups.insert(room->alternate_group());

                int16_t alternate = room->alternate_room();
                if (alternate != -1)
                {
                    _rooms[alternate]->set_is_alternate(i);
                }
            }
        }
    }

    void Level::generate_triggers()
    {
        for (auto i = 0; i < _rooms.size(); ++i)
        {
            const auto& room = _rooms[i];
            for (auto sector : room->sectors())
            {
                if (sector.second->flags & SectorFlag::Trigger)
                {
                    _triggers.emplace_back(std::make_unique<Trigger>(_triggers.size(), i, sector.second->x(), sector.second->z(), sector.second->trigger()));
                    room->add_trigger(_triggers.back().get());
                }
            }
        }

        for (auto& room : _rooms)
        {
            room->generate_trigger_geometry();
        }
    }

    void Level::generate_entities(const graphics::Device& device)
    {
        const uint32_t num_entities = _level->num_entities();
        for (uint32_t i = 0; i < num_entities; ++i)
        {
            // Entity for rendering.
            auto level_entity = _level->get_entity(i);
            auto entity = std::make_unique<Entity>(device, *_level, level_entity, *_texture_storage.get(), *_mesh_storage.get(), i);
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
            _items.emplace_back(i, level_entity.Room, level_entity.TypeID, lookup_type_name(level_entity.TypeID), _level->get_version() >= trlevel::LevelVersion::Tomb4 ? level_entity.Intensity2 : 0, level_entity.Flags, relevant_triggers);
        }
    }

    void Level::regenerate_neighbours()
    {
        _neighbours.clear();
        if (_selected_room < _level->num_rooms())
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
                choose(original_room->pick(position, direction, true, _show_triggers, _show_hidden_geometry, false));
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
    }

    void Level::set_alternate_group(uint16_t group, bool enabled)
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
    }

    bool Level::alternate_group(uint16_t group) const
    {
        return _alternate_groups.find(group) != _alternate_groups.end();
    }

    // Determines whether the alternate mode specified is a mismatch with the current setting of 
    // the alternate mode flag.
    bool Level::is_alternate_mismatch(const Room& room) const
    {
        if (_level->get_version() >= trlevel::LevelVersion::Tomb4)
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

    void Level::load_type_name_lookup()
    {
        Resource type_list = get_resource_memory(IDR_TYPE_NAMES, L"TEXT");

        auto contents = std::string(type_list.data, type_list.data + type_list.size);
        auto json = nlohmann::json::parse(contents.begin(), contents.end());

        std::string game_name;
        switch (_level->get_version())
        {
        case trlevel::LevelVersion::Tomb1:
            game_name = "tr1";
            break;
        case trlevel::LevelVersion::Tomb2:
            game_name = "tr2";
            break;
        case trlevel::LevelVersion::Tomb3:
            game_name = "tr3";
            break;
        case trlevel::LevelVersion::Tomb4:
            game_name = "tr4";
            break;
        case trlevel::LevelVersion::Tomb5:
            game_name = "tr5";
            break;
        default:
            return;
        }

        for (const auto& element : json["games"][game_name])
        {
            auto name = element.at("name").get<std::string>();
            _type_names.insert({ element.at("id").get<uint32_t>(), std::wstring(name.begin(), name.end()) });
        }
    }

    std::wstring Level::lookup_type_name(uint32_t type_id) const
    {
        const auto found = _type_names.find(type_id);
        if (found == _type_names.end())
        {
            return std::to_wstring(type_id);
        }
        return found->second;
    }

    void Level::set_show_triggers(bool show)
    {
        _show_triggers = show;
        _regenerate_transparency = true;
    }

    void Level::set_show_hidden_geometry(bool show)
    {
        _show_hidden_geometry = show;
    }

    bool Level::show_hidden_geometry() const
    {
        return _show_hidden_geometry;
    }

    void Level::set_show_water(bool show)
    {
        _show_water = show;
        _regenerate_transparency = true;
    }

    bool Level::show_triggers() const
    {
        return _show_triggers;
    }

    void Level::set_selected_trigger(uint32_t number)
    {
        _selected_trigger = _triggers[number].get();
    }

    const ILevelTextureStorage& Level::texture_storage() const
    {
        return *_texture_storage;
    }

    std::set<uint16_t> Level::alternate_groups() const
    {
        std::set<uint16_t> groups;
        for (int16_t i = 0; i < _rooms.size(); ++i)
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

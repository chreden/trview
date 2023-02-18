#include "Level.h"

#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IShader.h>

#include <trview.app/Graphics/LevelTextureStorage.h>
#include <trview.app/Camera/ICamera.h>
#include <trview.graphics/RasterizerStateStore.h>
#include <format>
#include <ranges>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
#pragma warning(push)
#pragma warning(disable : 4324)
#pragma pack(push, 16)
        __declspec(align(16)) struct PixelShaderData
        {
            bool disable_transparency;
        };
#pragma pack(pop)
#pragma warning(pop)
    }

    ILevel::~ILevel()
    {
    }

    Level::Level(const std::shared_ptr<graphics::IDevice>& device,
        const std::shared_ptr<graphics::IShaderStorage>& shader_storage,
        std::shared_ptr<ILevelTextureStorage> level_texture_storage,
        std::unique_ptr<ITransparencyBuffer> transparency_buffer,
        std::unique_ptr<ISelectionRenderer> selection_renderer,
        const std::shared_ptr<ILog>& log,
        const graphics::IBuffer::ConstantSource& buffer_source)
        : _device(device), _texture_storage(level_texture_storage),
        _transparency(std::move(transparency_buffer)), _selection_renderer(std::move(selection_renderer)), _log(log)
    {
        _vertex_shader = shader_storage->get("level_vertex_shader");
        _pixel_shader = shader_storage->get("level_pixel_shader");

        // Create a texture sampler state description.
        D3D11_SAMPLER_DESC sampler_desc;
        memset(&sampler_desc, 0, sizeof(sampler_desc));
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.MaxAnisotropy = 1;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

        D3D11_RASTERIZER_DESC default_rasterizer_desc;
        memset(&default_rasterizer_desc, 0, sizeof(default_rasterizer_desc));
        default_rasterizer_desc.FillMode = D3D11_FILL_SOLID;
        default_rasterizer_desc.CullMode = D3D11_CULL_BACK;
        default_rasterizer_desc.DepthClipEnable = true;
        default_rasterizer_desc.MultisampleEnable = true;
        default_rasterizer_desc.AntialiasedLineEnable = true;
        _default_rasterizer = device->create_rasterizer_state(default_rasterizer_desc);

        D3D11_RASTERIZER_DESC rasterizer_desc;
        memset(&rasterizer_desc, 0, sizeof(rasterizer_desc));
        rasterizer_desc.FillMode = D3D11_FILL_WIREFRAME;
        rasterizer_desc.CullMode = D3D11_CULL_BACK;
        rasterizer_desc.DepthClipEnable = true;
        _wireframe_rasterizer = device->create_rasterizer_state(rasterizer_desc);

        _pixel_shader_data = buffer_source(sizeof(PixelShaderData));

        // Create the texture sampler state.
        _sampler_state = device->create_sampler_state(sampler_desc);
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

    std::optional<uint32_t> Level::selected_item() const
    {
        if (auto item = _selected_item.lock())
        {
            return item->number();
        }
        return std::nullopt;
    }

    uint16_t Level::selected_room() const
    {
        return _selected_room;
    }

    std::weak_ptr<IItem> Level::item(uint32_t index) const
    {
        if (index >= _entities.size())
        {
            return {};
        }
        return _entities[index];
    }

    std::vector<std::weak_ptr<IItem>> Level::items() const
    {
        std::vector<std::weak_ptr<IItem>> entities;
        std::transform(_entities.begin(), _entities.end(), std::back_inserter(entities), [](auto&& entity) { return entity; });;
        return entities;
    }

    uint32_t Level::neighbour_depth() const
    {
        return _neighbour_depth;
    }

    uint32_t Level::number_of_rooms() const
    {
        return static_cast<uint32_t>(_rooms.size());
    }

    std::vector<std::weak_ptr<IRoom>> Level::rooms() const
    {
        std::vector<std::weak_ptr<IRoom>> rooms;
        std::transform(_rooms.begin(), _rooms.end(), std::back_inserter(rooms), [](auto&& room) { return room; });;
        return rooms;
    }

    std::weak_ptr<ITrigger> Level::trigger(uint32_t index) const
    {
        if (index >= _triggers.size())
        {
            return {};
        }
        return _triggers[index];
    }

    std::vector<std::weak_ptr<ITrigger>> Level::triggers() const
    {
        std::vector<std::weak_ptr<ITrigger>> triggers;
        std::transform(_triggers.begin(), _triggers.end(), std::back_inserter(triggers), [](const auto& trigger) { return trigger; });
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
        on_room_selected(index);
    }

    void Level::set_selected_item(uint32_t index)
    {
        _selected_item = _entities[index];
        on_level_changed();
    }

    void Level::set_neighbour_depth(uint32_t depth)
    {
        _neighbour_depth = depth;
        regenerate_neighbours();
        on_level_changed();
    }

    void Level::render(const ICamera& camera, bool render_selection)
    {
        using namespace DirectX;

        auto context = _device->context();

        {
            graphics::RasterizerStateStore rasterizer_store(context);
            context->PSSetSamplers(0, 1, _sampler_state.GetAddressOf());
            if (_show_wireframe)
            {
                context->RSSetState(_wireframe_rasterizer.Get());
            }
            else
            {
                context->RSSetState(_default_rasterizer.Get());
            }

            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            _vertex_shader->apply(context);
            _pixel_shader->apply(context);

            graphics::set_data(*_pixel_shader_data, context, PixelShaderData{ true });
            _pixel_shader_data->apply(context, graphics::IBuffer::ApplyTo::PS);

            render_rooms(camera);

            graphics::set_data(*_pixel_shader_data, context, PixelShaderData{ false });
        }

        if (render_selection)
        {
            render_selected_item(camera);
        }
    }

    // Render the rooms in the level.
    // context: The device context.
    // camera: The current camera to render the level with.
    void Level::render_rooms(const ICamera& camera)
    {
        // Only render the rooms that the current view mode includes.
        auto rooms = get_rooms_to_render(camera);

        if (_regenerate_transparency)
        {
            _transparency->reset();
        }

        std::unordered_set<uint32_t> visible_set;
        for (const auto& room : rooms)
        {
            visible_set.insert(room.number);
        }

        // Render the opaque portions of the rooms and also collect the transparent triangles
        // that need to be rendered in the second pass.
        for (const auto& room : rooms)
        {
            room.room.render(camera, room.selection_mode, _render_filters, visible_set);
            if (_regenerate_transparency)
            {
                room.room.get_transparent_triangles(*_transparency, camera, room.selection_mode, _render_filters);
            }

            // If this is an alternate room, render the items from the original room in the sample places.
            if (!is_alternate_mismatch(room.room) && room.room.alternate_mode() == IRoom::AlternateMode::IsAlternate)
            {
                auto& original_room = _rooms[room.room.alternate_room()];
                original_room->render_contained(camera, room.selection_mode, _render_filters);
                if (_regenerate_transparency)
                {
                    original_room->get_contained_transparent_triangles(*_transparency, camera, room.selection_mode, _render_filters);
                }
            }
        }

        if (has_flag(_render_filters, RenderFilter::BoundingBoxes))
        {
            const auto context = _device->context();
            graphics::RasterizerStateStore rasterizer_store(context);
            if (!_show_wireframe)
            {
                context->RSSetState(_wireframe_rasterizer.Get());
            }

            for (const auto& room : rooms)
            {
                room.room.render_bounding_boxes(camera);
            }
        }

        if (has_flag(_render_filters, RenderFilter::Lights))
        {
            for (const auto& room : rooms)
            {
                room.room.render_lights(camera, _selected_light);
            }
        }

        if (has_flag(_render_filters, RenderFilter::CameraSinks))
        {
            for (const auto& room : rooms)
            {
                room.room.render_camera_sinks(camera);
            }
        }

        if (_regenerate_transparency)
        {
            // Sort the accumulated transparent triangles farthest to nearest.
            _transparency->sort(camera.rendering_position());
        }

        _regenerate_transparency = false;
    }

    void Level::render_transparency(const ICamera& camera)
    {
        auto context = _device->context();
        graphics::RasterizerStateStore rasterizer_store(context);
        if (_show_wireframe)
        {
            context->RSSetState(_wireframe_rasterizer.Get());
        }

        graphics::set_data(*_pixel_shader_data, context, PixelShaderData{ false });
        _pixel_shader_data->apply(context, graphics::IBuffer::ApplyTo::PS);

        // Render the triangles that the transparency buffer has produced.
        _transparency->render(camera, *_texture_storage.get());
    }

    void Level::render_selected_item(const ICamera& camera)
    {
        const Color Trigger_Outline{ 0.0f, 1.0f, 0.0f, 1.0f };
        const Color Item_Outline{ 1.0f, 1.0f, 0.0f, 1.0f };

        auto selected_item = _selected_item.lock();
        if (selected_item)
        {
            _selection_renderer->render(camera, *_texture_storage, *selected_item, Item_Outline);
        }

        if (has_flag(_render_filters, RenderFilter::Triggers))
        {
            auto selected_trigger = _selected_trigger.lock();
            if (selected_trigger)
            {
                _selection_renderer->render(camera, *_texture_storage, *selected_trigger, Trigger_Outline);
            }
        }
    }

    // Get the collection of rooms that need to be renderered depending on the current view mode.
    // Returns: The rooms to render and their selection mode.
    std::vector<Level::RoomToRender> Level::get_rooms_to_render(const ICamera& camera) const
    {
        std::vector<RoomToRender> rooms;

        const auto frustum = camera.frustum();
        const auto view_projection = camera.view_projection();
        BoundingBox screen_box;
        DirectX::BoundingBox::CreateFromPoints(screen_box, Vector3(-1, -1, 0), Vector3(1, 1, 1));

        auto visible_perspective = [&](const IRoom& room)
        {
            return frustum.Contains(room.bounding_box()) != DISJOINT;
        };

        auto visible_orthographic = [&](const IRoom& room)
        {
            DirectX::BoundingBox room_box = room.bounding_box();
            room_box.Transform(room_box, view_projection);
            return room_box.Intersects(screen_box);
        };

        auto in_view = [&](const IRoom& room)
        {
            return camera.projection_mode() == ProjectionMode::Orthographic ? visible_orthographic(room) : visible_perspective(room);
        };

        bool highlight = highlight_mode_enabled(RoomHighlightMode::Highlight);
        if (highlight_mode_enabled(RoomHighlightMode::Neighbours))
        {
            for (uint16_t i : _neighbours)
            {
                const auto& room = _rooms[i];
                if (!room->visible() || is_alternate_mismatch(*room) || !in_view(*room))
                {
                    continue;
                }
                rooms.emplace_back(*room.get(), highlight ? (i == _selected_room ? IRoom::SelectionMode::Selected : IRoom::SelectionMode::NotSelected) : IRoom::SelectionMode::Selected, i);
            }
        }
        else
        {
            for (std::size_t i = 0; i < _rooms.size(); ++i)
            {
                const auto& room = _rooms[i].get();
                if (!room->visible() || is_alternate_mismatch(*room) || !in_view(*room))
                {
                    continue;
                }
                rooms.emplace_back(*room, highlight ? (_selected_room == static_cast<uint16_t>(i) ? IRoom::SelectionMode::Selected : IRoom::SelectionMode::NotSelected) : IRoom::SelectionMode::Selected, static_cast<uint16_t>(i));
            }
        }

        return rooms;
    }

    void Level::generate_rooms(const trlevel::ILevel& level, const IRoom::Source& room_source, const IMeshStorage& mesh_storage)
    {
        Activity generate_rooms_activity(_log, "Level", level.name());
        const auto num_rooms = level.num_rooms();
        for (uint32_t i = 0u; i < num_rooms; ++i)
        {
            Activity room_activity(generate_rooms_activity, std::format("Room {}", i));
            auto room = level.get_room(i);
            _rooms.push_back(room_source(level, room, _texture_storage, mesh_storage, i, shared_from_this(), room_activity));
        }

        std::set<uint32_t> alternate_groups;

        // Fix up the IsAlternate status of the rooms that are referenced by HasAlternate rooms.
        // This can only be done once all the rooms are loaded.
        for (auto i = 0u; i < _rooms.size(); ++i)
        {
            const auto& room = _rooms[i];
            if (room->alternate_mode() == IRoom::AlternateMode::HasAlternate)
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

    void Level::generate_triggers(const ITrigger::Source& trigger_source)
    {
        for (auto i = 0u; i < _rooms.size(); ++i)
        {
            const auto& room = _rooms[i];
            for (auto sector : room->sectors())
            {
                if (has_flag(sector->flags(), SectorFlag::Trigger))
                {
                    _triggers.push_back(trigger_source(static_cast<uint32_t>(_triggers.size()), i, sector->x(), sector->z(), sector->trigger(), _version));
                    room->add_trigger(_triggers.back());
                }
            }
        }

        for (auto& room : _rooms)
        {
            room->generate_trigger_geometry();
        }

        std::function<void(std::shared_ptr<ISector>)> add_monkey_swing;
        add_monkey_swing = [&](auto&& sector)
        {
            if (has_flag(sector->flags(), SectorFlag::MonkeySwing) && sector->room_above() != 0xff)
            {
                auto portal = _rooms[sector->room()]->sector_portal(sector->x(), sector->z(), -1, -1);
                if (has_flag(portal.sector_above->flags(), SectorFlag::MonkeySwing))
                {
                    return;
                }

                portal.sector_above->add_flag(SectorFlag::MonkeySwing);
                add_monkey_swing(portal.sector_above);
            }
        };

        std::function<void(std::shared_ptr<ISector>)> add_ladders;
        add_ladders = [&](auto&& sector)
        {
            if (has_any_flag(sector->flags(), SectorFlag::ClimbableNorth, SectorFlag::ClimbableSouth, SectorFlag::ClimbableWest, SectorFlag::ClimbableEast) && sector->room_above() != 0xff)
            {
                auto portal = _rooms[sector->room()]->sector_portal(sector->x(), sector->z(), -1, -1);
                portal.sector_above->add_flag(sector->flags() & SectorFlag::Climbable);
                add_ladders(portal.sector_above);
            }
        };

        // Propagate monkey bars
        for (auto& room : _rooms)
        {
            for (const auto& sector : room->sectors())
            {
                add_monkey_swing(sector);
                add_ladders(sector);
            }
        }

        for (auto& room : _rooms)
        {
            room->generate_sector_triangles();
        }

        deduplicate_triangles();
    }

    void Level::generate_entities(const trlevel::ILevel& level, const IItem::EntitySource& entity_source, const IItem::AiSource& ai_source, const IMeshStorage& mesh_storage)
    {
        const uint32_t num_entities = level.num_entities();
        for (uint32_t i = 0; i < num_entities; ++i)
        {
            std::vector<std::weak_ptr<ITrigger>> relevant_triggers;
            for (const auto& trigger : _triggers)
            {
                if (trigger->triggers_item(i))
                {
                    relevant_triggers.push_back(trigger);
                }
            }

            auto level_entity = level.get_entity(i);
            auto entity = entity_source(level, level_entity, i, relevant_triggers, mesh_storage, shared_from_this());
            _rooms[entity->room()]->add_entity(entity);
            _entities.push_back(entity);
        }

        const uint32_t num_ai_objects = level.num_ai_objects();
        for (uint32_t i = 0; i < num_ai_objects; ++i)
        {
            auto ai_object = level.get_ai_object(i);
            auto entity = ai_source(level, ai_object, num_entities + i, mesh_storage, shared_from_this());
            _rooms[entity->room()]->add_entity(entity);
            _entities.push_back(entity);
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
    PickResult Level::pick(const ICamera& camera, const Vector3& position, const Vector3& direction) const
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
                final_result.centroid = result.centroid;
                final_result.index = result.index;
                final_result.type = result.type;
                final_result.triangle = result.triangle;
            }
        };

        auto rooms = get_rooms_to_render(camera);
        for (auto& room : rooms)
        {
            choose(room.room.pick(position, direction,
                filter_flag(PickFilter::Geometry, has_flag(_render_filters, RenderFilter::Rooms)) |
                filter_flag(PickFilter::Entities, has_flag(_render_filters, RenderFilter::Entities)) |
                filter_flag(PickFilter::StaticMeshes, has_flag(_render_filters, RenderFilter::Rooms)) |
                filter_flag(PickFilter::AllGeometry, has_flag(_render_filters, RenderFilter::AllGeometry)) |
                filter_flag(PickFilter::Triggers, has_flag(_render_filters, RenderFilter::Triggers)) |
                filter_flag(PickFilter::Lights, has_flag(_render_filters, RenderFilter::Lights)) |
                filter_flag(PickFilter::CameraSinks, has_flag(_render_filters, RenderFilter::CameraSinks))));
            if (!is_alternate_mismatch(room.room) && room.room.alternate_mode() == IRoom::AlternateMode::IsAlternate)
            {
                auto& original_room = _rooms[room.room.alternate_room()];
                choose(original_room->pick(position, direction, PickFilter::Entities));
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

    void Level::set_item_visibility(uint32_t index, bool state)
    {
        _entities[index]->set_visible(state);
        _regenerate_transparency = true;
        on_level_changed();
    }

    void Level::set_trigger_visibility(uint32_t index, bool state)
    {
        _triggers[index]->set_visible(state);
        _regenerate_transparency = true;
        on_level_changed();
    }

    // Set whether to render the alternate mode (the flipmap) or the regular room.
    // enabled: Whether to render the flipmap.
    void Level::set_alternate_mode(bool enabled)
    {
        if (_alternate_mode == enabled)
        {
            return;
        }

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
        on_alternate_mode_selected(enabled);
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
    bool Level::is_alternate_mismatch(const IRoom& room) const
    {
        if (version() >= trlevel::LevelVersion::Tomb4)
        {
            return room.alternate_mode() == IRoom::AlternateMode::HasAlternate && is_alternate_group_set(room.alternate_group()) ||
                   room.alternate_mode() == IRoom::AlternateMode::IsAlternate && !is_alternate_group_set(room.alternate_group());
        }

        return room.alternate_mode() == IRoom::AlternateMode::IsAlternate && !_alternate_mode ||
               room.alternate_mode() == IRoom::AlternateMode::HasAlternate && _alternate_mode;
    }

    // Get the current state of the alternate mode (flipmap).
    bool Level::alternate_mode() const
    {
        return _alternate_mode;
    }

    bool Level::any_alternates() const
    {
        return std::any_of(_rooms.begin(), _rooms.end(), [](const std::shared_ptr<IRoom>& room)
        {
            return room->alternate_mode() != IRoom::AlternateMode::None;
        });
    }

    void Level::set_show_triggers(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::Triggers, show);
        _regenerate_transparency = true;
        on_level_changed();
    }

    void Level::set_show_geometry(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::AllGeometry, show);
        _regenerate_transparency = true;
        on_level_changed();
    }

    bool Level::show_geometry() const
    {
        return has_flag(_render_filters, RenderFilter::AllGeometry);
    }

    void Level::set_show_water(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::Water, show);
        _regenerate_transparency = true;
        on_level_changed();
    }

    void Level::set_show_wireframe(bool show)
    {
        _show_wireframe = show;
        _regenerate_transparency = true;
        on_level_changed();
    }

    void Level::set_show_bounding_boxes(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::BoundingBoxes, show);
        _regenerate_transparency = true;
        on_level_changed();
    }

    void Level::set_show_lights(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::Lights, show);
        _regenerate_transparency = true;
        on_level_changed();
    }

    void Level::set_show_items(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::Entities, show);
        _regenerate_transparency = true;
        on_level_changed();
    }

    void Level::set_show_rooms(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::Rooms, show);
        _regenerate_transparency = true;
        on_level_changed();
    }

    bool Level::show_triggers() const
    {
        return has_flag(_render_filters, RenderFilter::Triggers);
    }

    bool Level::show_lights() const
    {
        return has_flag(_render_filters, RenderFilter::Lights);
    }

    bool Level::show_items() const
    {
        return has_flag(_render_filters, RenderFilter::Entities);
    }

    void Level::set_selected_trigger(uint32_t number)
    {
        _selected_trigger = _triggers[number];
        on_level_changed();
    }

    void Level::set_selected_light(uint32_t number)
    {
        _selected_light = _lights[number];
        on_level_changed();
    }

    void Level::set_selected_camera_sink(uint32_t number)
    {
        _selected_camera_sink = _camera_sinks[number];
        on_level_changed();
    }

    std::shared_ptr<ILevelTextureStorage> Level::texture_storage() const
    {
        return _texture_storage;
    }

    std::set<uint32_t> Level::alternate_groups() const
    {
        std::set<uint32_t> groups;
        for (auto i = 0u; i < _rooms.size(); ++i)
        {
            const auto& room = _rooms[i];
            if (room->alternate_mode() != IRoom::AlternateMode::None)
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

    std::string Level::filename() const
    {
        return _filename;
    }

    void Level::set_filename(const std::string& filename)
    {
        _filename = filename;
    }

    std::weak_ptr<IRoom> Level::room(uint32_t id) const
    {
        if (id >= _rooms.size())
        {
            return {};
        }
        return _rooms[id];
    }

    void Level::apply_ocb_adjustment()
    {
        for (auto& entity : _entities)
        {
            if (!entity->needs_ocb_adjustment())
            {
                continue;
            }

            const auto entity_pos = entity->bounding_box().Center;
            const auto result = _rooms[entity->room()]->pick(Vector3(entity_pos.x, entity_pos.y, entity_pos.z), Vector3(0, 1, 0), PickFilter::Geometry | PickFilter::StaticMeshes);
            if (result.hit)
            {
                const auto new_height = result.position.y - entity->bounding_box().Extents.y;
                entity->adjust_y(new_height - entity_pos.y);
            }
        }
    }

    std::vector<uint16_t> Level::floor_data() const
    {
        return _floor_data;
    }

    void Level::generate_lights(const trlevel::ILevel& level, const ILight::Source& light_source)
    {
        const auto num_rooms = level.num_rooms();
        for (uint32_t i = 0u; i < num_rooms; ++i)
        {
            auto room = level.get_room(i);
            for (const auto& light : room.lights)
            {
                _lights.push_back(light_source(static_cast<uint32_t>(_lights.size()), i, light));
                _rooms[i]->add_light(_lights.back());
            }
        }
    }

    std::weak_ptr<ILight> Level::light(uint32_t index) const
    {
        if (index >= _lights.size())
        {
            return {};
        }
        return _lights[index];
    }

    std::vector<std::weak_ptr<ILight>> Level::lights() const
    {
        std::vector<std::weak_ptr<ILight>> lights;
        std::transform(_lights.begin(), _lights.end(), std::back_inserter(lights), [](const auto& light) { return light; });
        return lights;
    }

    void Level::set_light_visibility(uint32_t index, bool state)
    {
        _lights[index]->set_visible(state);
        _regenerate_transparency = true;
        on_level_changed();
    }

    void Level::set_room_visibility(uint32_t index, bool state)
    {
        _rooms[index]->set_visible(state);
        _regenerate_transparency = true;
        on_level_changed();
    }

    void Level::set_camera_sink_visibility(uint32_t index, bool state)
    {
        _camera_sinks[index]->set_visible(state);
        _regenerate_transparency = true;
        on_level_changed();
    }

    void Level::deduplicate_triangles()
    {
        struct TriangleData
        {
            std::vector<ISector::Triangle> room_triangles;
            std::vector<uint32_t> triangle_rooms;
            std::vector<uint32_t> sector_tri_counts;
        };

        std::vector<TriangleData> all_data;
        for (const auto& room : _rooms)
        {
            TriangleData data;
            const auto info = room->info();
            const auto offset = Vector3(info.x / trlevel::Scale_X, 0, info.z / trlevel::Scale_Z);

            for (const auto& sector : room->sectors())
            {
                const auto tris = sector->triangles();
                std::transform(tris.begin(), tris.end(), std::back_inserter(data.room_triangles), [&](const auto& t) 
                    {
                        auto t2 = t + offset;
                        t2.room = room->number();
                        return t2; 
                    });
                data.sector_tri_counts.push_back(static_cast<uint32_t>(tris.size()));
            }
            data.triangle_rooms.resize(data.room_triangles.size(), room->number());
            all_data.push_back(data);
        }

        for (auto r = 0u; r < _rooms.size(); ++r)
        {
            for (const auto& neighbour : _rooms[r]->neighbours())
            {
                for (auto t = 0u; t < all_data[r].room_triangles.size(); ++t)
                {
                    for (auto t2 = 0u; t2 < all_data[neighbour].room_triangles.size(); ++t2)
                    {
                        if (all_data[r].room_triangles[t] == all_data[neighbour].room_triangles[t2])
                        {
                            all_data[r].triangle_rooms[t] = neighbour;
                            all_data[neighbour].triangle_rooms[t2] = r;
                        }
                    }
                }
            }
        }

        for (uint32_t i = 0; i < _rooms.size(); ++i)
        {
            _rooms[i]->set_sector_triangle_rooms(all_data[i].triangle_rooms);
        }
    }

    MapColours Level::map_colours() const
    {
        return _map_colours;
    }
   
    void Level::set_map_colours(const MapColours& map_colours)
    {
        _map_colours = map_colours;
        on_geometry_colours_changed();
    }

    std::optional<uint32_t> Level::selected_light() const
    {
        if (auto light = _selected_light.lock())
        {
            return light->number();
        }
        return std::nullopt;
    }

    std::optional<uint32_t> Level::selected_trigger() const
    {
        if (auto trigger = _selected_trigger.lock())
        {
            return trigger->number();
        }
        return std::nullopt;
    }

    bool Level::has_model(uint32_t type_id) const
    {
        return _models.find(type_id) != _models.end();
    }

    void Level::record_models(const trlevel::ILevel& level)
    {
        for (uint32_t i = 0; i < level.num_models(); ++i)
        {
            _models.insert(level.get_model(i).ID);
        }
    }

    std::weak_ptr<ICameraSink> Level::camera_sink(uint32_t index) const
    {
        if (index >= _camera_sinks.size())
        {
            return {};
        }
        return _camera_sinks[index];
    }

    std::vector<std::weak_ptr<ICameraSink>> Level::camera_sinks() const
    {
        std::vector<std::weak_ptr<ICameraSink>> camera_sinks;
        std::transform(_camera_sinks.begin(), _camera_sinks.end(), std::back_inserter(camera_sinks), [](const auto& camera_sink) { return camera_sink; });
        return camera_sinks;
    }

    void Level::generate_camera_sinks(const trlevel::ILevel& level, const ICameraSink::Source& camera_sink_source)
    {
        for (uint32_t i = 0u; i < level.num_cameras(); ++i)
        {
            const auto camera_sink = level.get_camera(i);
            const Vector3 point = Vector3(
                static_cast<float>(camera_sink.x),
                static_cast<float>(camera_sink.y),
                static_cast<float>(camera_sink.z)) / trlevel::Scale;

            const bool is_camera = std::ranges::any_of(_triggers, [=](auto&& trigger)
                {
                    return std::ranges::any_of(trigger->commands(), [=](const Command& command)
                        {
                            return command.type() == TriggerCommandType::Camera && command.index() == i;
                        });
                });

            std::vector<uint16_t> in_space_rooms;
            std::vector<uint16_t> in_portal_rooms;

            for (const auto& room : _rooms)
            {
                if (std::shared_ptr<ISector> sector = sector_from_point(*room, point))
                {
                    if (sector && sector->is_portal())
                    {
                        in_portal_rooms.push_back(static_cast<uint16_t>(room->number()));
                    }
                    else
                    {
                        in_space_rooms.push_back(static_cast<uint16_t>(room->number()));
                    }
                }
            }

            std::vector<std::weak_ptr<ITrigger>> relevant_triggers;
            for (const auto& trigger : _triggers)
            {
                if (std::ranges::any_of(trigger->commands(), [&](const auto& command) { return command.index() == i && equals_any(command.type(), TriggerCommandType::UnderwaterCurrent, TriggerCommandType::Camera); }))
                {
                    relevant_triggers.push_back(trigger);
                }
            }

            const std::vector<uint16_t> inferred_rooms{ in_space_rooms.empty() ? in_portal_rooms : in_space_rooms };

            const ICameraSink::Type type = is_camera ? ICameraSink::Type::Camera : ICameraSink::Type::Sink;
            auto new_camera_sink = camera_sink_source(i, camera_sink, type, inferred_rooms, relevant_triggers);
            _camera_sinks.push_back(new_camera_sink);

            if (is_camera)
            {
                _rooms[camera_sink.Room]->add_camera_sink(new_camera_sink);
            }
            else
            {
                std::ranges::for_each(inferred_rooms, [&](auto&& r) { _rooms[r]->add_camera_sink(new_camera_sink); });
            }
        }
    }

    void Level::set_show_camera_sinks(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::CameraSinks, show);
        _regenerate_transparency = true;
        on_level_changed();
    }

    std::optional<uint32_t> Level::selected_camera_sink() const
    {
        if (auto camera_sink = _selected_camera_sink.lock())
        {
            return camera_sink->number();
        }
        return std::nullopt;
    }

    bool Level::show_camera_sinks() const
    {
        return has_flag(_render_filters, RenderFilter::CameraSinks);
    }

    void Level::initialise(std::unique_ptr<trlevel::ILevel> level,
        std::unique_ptr<IMeshStorage> mesh_storage,
        const IItem::EntitySource& entity_source,
        const IItem::AiSource& ai_source,
        const IRoom::Source& room_source,
        const ITrigger::Source& trigger_source,
        const ILight::Source& light_source,
        const ICameraSink::Source& camera_sink_source)
    {
        _version = level->get_version();
        _floor_data = level->get_floor_data_all();

        record_models(*level);
        generate_rooms(*level, room_source, *mesh_storage);
        generate_triggers(trigger_source);
        generate_entities(*level, entity_source, ai_source, *mesh_storage);
        generate_lights(*level, light_source);
        generate_camera_sinks(*level, camera_sink_source);

        for (auto& room : _rooms)
        {
            room->update_bounding_box();
        }

        apply_ocb_adjustment();

        for (auto& trigger : _triggers)
        {
            trigger->set_level(shared_from_this());
        }

        for (auto& camera_sink : _camera_sinks)
        {
            camera_sink->set_level(shared_from_this());
        }

        for (auto& light : _lights)
        {
            light->set_level(shared_from_this());
        }
    }

    bool find_item_by_type_id(const ILevel& level, uint32_t type_id, std::weak_ptr<IItem>& output_item)
    {
        const auto& items = level.items();
        auto found_item = std::find_if(items.begin(), items.end(), [=](const auto& item)
            {
                auto i = item.lock();
                return i && i->type_id() == type_id; 
            });
        if (found_item == items.end())
        {
            return false;
        }
        output_item = *found_item;
        return true;
    }

    bool find_last_item_by_type_id(const ILevel& level, uint32_t type_id, std::weak_ptr<IItem>& output_item)
    {
        const auto& items = level.items();
        auto found_item = std::find_if(items.rbegin(), items.rend(), [=](const auto& item)
            {
                auto i = item.lock();
                return i && i->type_id() == type_id;
            });
        if (found_item == items.rend())
        {
            return false;
        }
        output_item = *found_item;
        return true;
    }
}

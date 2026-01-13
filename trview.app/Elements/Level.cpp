#include "Level.h"

#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IShader.h>
#include "../Elements/Flyby/IFlybyNode.h"

#include "../Graphics/LevelTextureStorage.h"
#include "../Camera/ICamera.h"
#include "Remastered/INgPlusSwitcher.h"
#include <trview.graphics/RasterizerStateStore.h>
#include <format>
#include <ranges>

#include "../Settings/UserSettings.h"
#include "../Messages/Messages.h"

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

        constexpr uint16_t get_skidoo(trlevel::PlatformAndVersion version)
        {
            if (version.version != trlevel::LevelVersion::Tomb2)
            {
                return 0;
            }

            if (trlevel::is_tr2_e3(version))
            {
                return 48;
            }

            return 51;
        }

        constexpr bool is_skidoo_driver(trlevel::PlatformAndVersion version, uint16_t id)
        {
            if (version.version != trlevel::LevelVersion::Tomb2)
            {
                return false;
            }

            if (trlevel::is_tr2_e3(version))
            {
                return id == 49;
            }

            return id == 52;
        }

        void add_ladder_up(const std::shared_ptr<ISector>& sector, SectorFlag flag)
        {
            if (auto room = sector->room().lock())
            {
                sector->add_flag(flag);
                auto portal = room->sector_portal(sector->x(), sector->z(), -1, -1);
                if (portal.sector_above)
                {
                    add_ladder_up(portal.sector_above, flag);
                }
            }
        }

        void add_ladder_down(const std::shared_ptr<ISector>& sector, SectorFlag flag)
        {
            if (auto room = sector->room().lock())
            {
                auto portal = room->sector_portal(sector->x(), sector->z(), -1, -1);
                if (portal.sector_below)
                {
                    add_ladder_down(portal.sector_below, flag);
                }
                else
                {
                    add_ladder_up(sector, flag);
                }
            }
        }

        void add_ladders(const std::shared_ptr<ISector>& sector)
        {
            if (has_any_flag(sector->flags(), SectorFlag::ClimbableNorth, SectorFlag::ClimbableSouth, SectorFlag::ClimbableWest, SectorFlag::ClimbableEast))
            {
                add_ladder_down(sector, sector->flags() & SectorFlag::Climbable);
            }
        }

        template <typename T>
        bool is_matching_level(const std::shared_ptr<T>& element, const ILevel* const level)
        {
            const auto level_ptr = element->level().lock();
            return level_ptr && level_ptr.get() == level;
        }
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
        const graphics::IBuffer::ConstantSource& buffer_source,
        std::shared_ptr<ISoundStorage> sound_storage,
        std::shared_ptr<INgPlusSwitcher> ngplus_switcher,
        const std::shared_ptr<graphics::ISamplerState>& sampler_state,
        const std::weak_ptr<IMessageSystem>& messaging)
        : _device(device), _texture_storage(level_texture_storage),
        _transparency(std::move(transparency_buffer)), _selection_renderer(std::move(selection_renderer)), _log(log), _sound_storage(sound_storage),
        _ngplus_switcher(ngplus_switcher), _room_sampler_state(sampler_state), _messaging(messaging)
    {
        _vertex_shader = shader_storage->get("level_vertex_shader");
        _pixel_shader = shader_storage->get("level_pixel_shader");

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

    std::weak_ptr<IRoom> Level::selected_room() const
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

    std::string Level::name() const
    {
        return _name;
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
    }

    bool Level::highlight_mode_enabled(RoomHighlightMode mode) const
    {
        return _room_highlight_modes.find(mode) != _room_highlight_modes.end();
    }

    std::vector<std::weak_ptr<IScriptable>> Level::scriptables() const
    {
        return _scriptables;
    }

    void Level::set_selected_room(const std::weak_ptr<IRoom>& room)
    { 
        const auto room_ptr = room.lock();
        if (_selected_room.lock() == room_ptr ||
            !is_matching_level(room_ptr, this))
        {
            return;
        }

        _selected_room = room;
        regenerate_neighbours();

        if (room_ptr && is_alternate_mismatch(*room_ptr))
        {
            if (version() >= trlevel::LevelVersion::Tomb4)
            {
                on_alternate_group_selected(room_ptr->alternate_group(), !is_alternate_group_set(room_ptr->alternate_group()));
            }
            else
            {
                on_alternate_mode_selected(!_alternate_mode);
            }
        }
    }

    void Level::set_selected_item(const std::weak_ptr<IItem>& item)
    {
        const auto selected_item = item.lock();
        if (_selected_item.lock() != selected_item && is_matching_level(selected_item, this))
        {
            sync_room(selected_item);
            _selected_item = selected_item;
        }
    }

    void Level::set_neighbour_depth(uint32_t depth)
    {
        _neighbour_depth = depth;
        regenerate_neighbours();
    }

    trlevel::Platform Level::platform() const
    {
        return _platform_and_version.platform;
    }

    trlevel::PlatformAndVersion Level::platform_and_version() const
    {
        return _platform_and_version;
    }

    void Level::render(const ICamera& camera, bool render_selection)
    {
        using namespace DirectX;

        auto context = _device->context();

        {
            graphics::RasterizerStateStore rasterizer_store(context);
            _room_sampler_state->apply();
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

            for (const auto& scriptable : _scriptables)
            {
                if (const auto scriptable_ptr = scriptable.lock())
                {
                    scriptable_ptr->render(camera);
                }
            }

            if (has_flag(_render_filters, RenderFilter::CameraSinks))
            {
                for (const auto& flyby : _flybys)
                {
                    flyby->render(camera, Colour::White);
                }
            }

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

        if (has_flag(_render_filters, RenderFilter::SoundSources))
        {
            for (const auto sound_source : _sound_sources)
            {
                sound_source->render(camera, Colour::White);
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

        _room_sampler_state->apply();
        _transparency->render(camera);
    }

    void Level::render_selected_item(const ICamera& camera)
    {
        const Color Trigger_Outline{ 0.0f, 1.0f, 0.0f, 1.0f };
        const Color Item_Outline{ 1.0f, 1.0f, 0.0f, 1.0f };

        auto selected_item = _selected_item.lock();
        if (selected_item)
        {
            _selection_renderer->render(camera, *selected_item, Item_Outline);
        }

        if (has_flag(_render_filters, RenderFilter::Triggers))
        {
            auto selected_trigger = _selected_trigger.lock();
            if (selected_trigger)
            {
                _selection_renderer->render(camera, *selected_trigger, Trigger_Outline);
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
        const auto selected = _selected_room.lock();
        if (highlight_mode_enabled(RoomHighlightMode::Neighbours))
        {
            for (uint16_t i : _neighbours)
            {
                const auto& room = _rooms[i];
                if (!room->visible() || is_alternate_mismatch(*room) || !in_view(*room))
                {
                    continue;
                }
                rooms.emplace_back(*room.get(), highlight ? (room == selected ? IRoom::SelectionMode::Selected : IRoom::SelectionMode::NotSelected) : IRoom::SelectionMode::Selected, i);
            }
        }
        else
        {
            for (std::size_t i = 0; i < _rooms.size(); ++i)
            {
                const auto& room = _rooms[i];
                if (!room->visible() || is_alternate_mismatch(*room) || !in_view(*room))
                {
                    continue;
                }
                rooms.emplace_back(*room, highlight ? (room == selected ? IRoom::SelectionMode::Selected : IRoom::SelectionMode::NotSelected) : IRoom::SelectionMode::Selected, static_cast<uint16_t>(i));
            }
        }

        return rooms;
    }

    void Level::generate_rooms(const trlevel::ILevel& level, const IRoom::Source& room_source, const IMeshStorage& mesh_storage)
    {
        Activity generate_rooms_activity(_log, "Level", level.name());
        const auto num_rooms = level.num_rooms();
        uint32_t sector_base_index = 0;
        for (uint32_t i = 0u; i < num_rooms; ++i)
        {
            Activity room_activity(generate_rooms_activity, std::format("Room {}", i));
            auto room = room_source(level, level.get_room(i), _texture_storage, mesh_storage, i, shared_from_this(), sector_base_index, room_activity);
            _token_store += room->on_changed += [this]() { content_changed(); };
            _rooms.push_back(room);
            sector_base_index += static_cast<uint32_t>(room->sectors().size());
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
        for (const auto& room : _rooms)
        {
            for (auto sector : room->sectors())
            {
                if (has_flag(sector->flags(), SectorFlag::Trigger))
                {
                    auto trigger = trigger_source(static_cast<uint32_t>(_triggers.size()), room, sector->x(), sector->z(), sector->trigger_info(), _platform_and_version.version, shared_from_this(), sector);
                    _triggers.push_back(trigger);
                    sector->set_trigger(trigger);
                    room->add_trigger(trigger);
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
                if (auto room = sector->room().lock())
                {
                    auto portal = room->sector_portal(sector->x(), sector->z(), -1, -1);
                    if (portal.sector_above)
                    {
                        if (has_flag(portal.sector_above->flags(), SectorFlag::MonkeySwing))
                        {
                            return;
                        }

                        portal.sector_above->add_flag(SectorFlag::MonkeySwing);
                        add_monkey_swing(portal.sector_above);
                    }
                }
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

    void Level::generate_entities(const trlevel::ILevel& level, const IItem::EntitySource& entity_source, const IItem::AiSource& ai_source, const IModelStorage& model_storage, const trlevel::ILevel::LoadCallbacks& callbacks)
    {
        std::vector<std::weak_ptr<IItem>> skidoo_drivers;

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
            auto containing_room = room(level_entity.Room);
            auto entity = entity_source(level, level_entity, i, relevant_triggers, model_storage, shared_from_this(), containing_room);
            if (auto room = containing_room.lock())
            {
                room->add_entity(entity);
            }
            _token_store += entity->on_changed += [this]() { content_changed(); };
            _entities.push_back(entity);

            if (is_skidoo_driver(level.platform_and_version(), level_entity.TypeID))
            {
                skidoo_drivers.push_back(entity);
            }
        }

        if (_platform_and_version.remastered)
        {
            callbacks.on_progress("Generating NG+ items");
            const auto extras = _ngplus_switcher->extras_for_level(shared_from_this(), level, model_storage);
            for (const auto& extra : extras)
            {
                if (auto room = extra->room().lock())
                {
                    room->add_entity(extra);
                }
                _token_store += extra->on_changed += [this]() { content_changed(); };
                _entities.push_back(extra);
            }

            const auto swapset = _ngplus_switcher->create_for_level(shared_from_this(), level, model_storage);
            for (const auto& [key, value] : swapset)
            {
                if (key > _entities.size())
                {
                    continue;
                }

                if (value)
                {
                    const auto existing_item = _entities[key];
                    if (auto room = existing_item->room().lock())
                    {
                        room->add_entity(value);
                    }
                    _token_store += value->on_changed += [this]() { content_changed(); };
                    _entities.push_back(value);
                }
            }
        }

        const uint32_t num_ai_objects = level.num_ai_objects();
        for (uint32_t i = 0; i < num_ai_objects; ++i)
        {
            auto ai_object = level.get_ai_object(i);
            auto containing_room = room(ai_object.room);
            auto entity = ai_source(level, ai_object, static_cast<uint32_t>(_entities.size()), model_storage, shared_from_this(), containing_room);
            if (auto room = containing_room.lock())
            {
                room->add_entity(entity);
            }
            _token_store += entity->on_changed += [this]() { content_changed(); };
            _entities.push_back(entity);
        }

        for (const auto& driver : skidoo_drivers)
        {
            if (auto man = driver.lock())
            {
                auto level_entity = level.get_entity(man->number());
                level_entity.TypeID = get_skidoo(level.platform_and_version());
                auto containing_room = man->room();
                auto entity = entity_source(level, level_entity, static_cast<uint32_t>(_entities.size()), {}, model_storage, shared_from_this(), containing_room);
                if (auto room = containing_room.lock())
                {
                    room->add_entity(entity);
                }
                auto categories = entity->categories();
                categories.insert("Virtual");
                entity->set_categories(categories);
                _token_store += entity->on_changed += [this]() { content_changed(); };
                _entities.push_back(entity);
            }
        }
    }

    void Level::regenerate_neighbours()
    {
        _neighbours.clear();
        if (auto selected_room = _selected_room.lock())
        {
            generate_neighbours(_neighbours, static_cast<uint16_t>(selected_room->number()), _neighbour_depth);
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
                if (room < _rooms.size())
                {
                    // Add the room to the final list of rooms.
                    results.insert(room);

                    // Get the neighbours of the room and add them to the next batch to be processed.
                    const auto neighbours = _rooms[room]->neighbours();
                    next_batch.insert(neighbours.begin(), neighbours.end());
                }
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
        std::vector<PickResult> results;
        for (auto& room : get_rooms_to_render(camera))
        {
            for (const auto& result : room.room.pick(position, direction,
                filter_flag(PickFilter::Geometry, has_flag(_render_filters, RenderFilter::Rooms)) |
                filter_flag(PickFilter::Entities, has_flag(_render_filters, RenderFilter::Entities)) |
                filter_flag(PickFilter::StaticMeshes, has_flag(_render_filters, RenderFilter::Rooms)) |
                filter_flag(PickFilter::AllGeometry, has_flag(_render_filters, RenderFilter::AllGeometry)) |
                filter_flag(PickFilter::Triggers, has_flag(_render_filters, RenderFilter::Triggers)) |
                filter_flag(PickFilter::Lights, has_flag(_render_filters, RenderFilter::Lights)) |
                filter_flag(PickFilter::CameraSinks, has_flag(_render_filters, RenderFilter::CameraSinks)) |
                filter_flag(PickFilter::NgPlus, has_flag(_render_filters, RenderFilter::NgPlus))))
            {
                results.push_back(result);
            }

            if (!is_alternate_mismatch(room.room) && room.room.alternate_mode() == IRoom::AlternateMode::IsAlternate)
            {
                const auto& original_room = _rooms[room.room.alternate_room()];
                for (const auto& result : original_room->pick(position, direction, PickFilter::Entities))
                {
                    results.push_back(result);
                }
            }
        }

        if (has_flag(_render_filters, RenderFilter::SoundSources))
        {
            for (const auto& sound_source : _sound_sources)
            {
                if (!sound_source->visible())
                {
                    continue;
                }

                auto sound_source_result = sound_source->pick(position, direction);
                if (sound_source_result.hit)
                {
                    results.push_back(sound_source_result);
                }
            }
        }

        if (has_flag(_render_filters, RenderFilter::CameraSinks))
        {
            for (const auto& flyby : _flybys)
            {
                if (!flyby->visible())
                {
                    continue;
                }

                auto flyby_result = flyby->pick(position, direction);
                if (flyby_result.hit)
                {
                    results.push_back(flyby_result);
                }
            }
        }


        uint32_t index = 0;
        for (const auto& scriptable : _scriptables)
        {
            if (const auto scriptable_ptr = scriptable.lock())
            {
                PickResult result{};
                BoundingBox cube(scriptable_ptr->position(), Vector3(0.125f, 0.125f, 0.125f));

                float distance = 0;
                if (cube.Intersects(position, direction, distance))
                {
                    result.distance = distance;
                    result.hit = true;
                    result.position = position + direction * distance;
                    result.type = PickResult::Type::Scriptable;
                    result.scriptable = scriptable;
                    results.push_back(result);
                }
            }

            ++index;
        }

        std::sort(results.begin(), results.end(), [](const auto& l, const auto& r) { return l.distance < r.distance; });

        std::optional<PickResult> actual_result;
        if (!results.empty())
        {
            actual_result = results.front();
        }

        for (const auto& result : results)
        {
            if (result.type == PickResult::Type::Room)
            {
                return actual_result.value_or(result);
            }

            if (result.type == PickResult::Type::Entity)
            {
                actual_result = result;
            }
        }

        return actual_result.value_or(PickResult {});
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
        if (_alternate_mode == enabled)
        {
            return;
        }

        _alternate_mode = enabled;
        _regenerate_transparency = true;

        // If the currently selected room is a room involved in flipmaps, select the alternate
        // room so that the user doesn't have an invisible room selected.
        const auto current_room = selected_room().lock();
        if (current_room && is_alternate_mismatch(*current_room))
        {
            if (auto level = current_room->level().lock())
            {
                messages::send_select_room(_messaging, level->room(current_room->alternate_room()));
            }
        }

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
        const auto current_room = selected_room().lock();
        if (current_room && is_alternate_mismatch(*current_room))
        {
            if (auto level = current_room->level().lock())
            {
                messages::send_select_room(_messaging, level->room(current_room->alternate_room()));
            }
        }
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
            return (room.alternate_mode() == IRoom::AlternateMode::HasAlternate && is_alternate_group_set(room.alternate_group()) && room.alternate_room() != -1) ||
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
    }

    void Level::set_show_geometry(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::AllGeometry, show);
        _regenerate_transparency = true;
    }

    bool Level::show_geometry() const
    {
        return has_flag(_render_filters, RenderFilter::AllGeometry);
    }

    void Level::set_show_water(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::Water, show);
        _regenerate_transparency = true;
    }

    void Level::set_show_wireframe(bool show)
    {
        _show_wireframe = show;
        _regenerate_transparency = true;
    }

    void Level::set_show_bounding_boxes(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::BoundingBoxes, show);
        _regenerate_transparency = true;
    }

    void Level::set_show_lighting(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::Lighting, show);
        _regenerate_transparency = true;
    }

    void Level::set_show_lights(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::Lights, show);
        _regenerate_transparency = true;
    }

    void Level::set_show_items(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::Entities, show);
        _regenerate_transparency = true;
    }

    void Level::set_show_rooms(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::Rooms, show);
        _regenerate_transparency = true;
    }

    bool Level::show_triggers() const
    {
        return has_flag(_render_filters, RenderFilter::Triggers);
    }

    bool Level::show_lighting() const
    {
        return has_flag(_render_filters, RenderFilter::Lighting);
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
        set_selected_trigger(_triggers[number]);
    }

    void Level::set_selected_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        const auto selected_trigger = trigger.lock();
        if (_selected_trigger.lock() != selected_trigger && is_matching_level(selected_trigger, this))
        {
            sync_room(selected_trigger);
            _selected_trigger = selected_trigger;
        }
    }

    void Level::set_selected_light(uint32_t number)
    {
        set_selected_light(_lights[number]);
    }

    void Level::set_selected_light(const std::weak_ptr<ILight>& light)
    {
        const auto selected_light = light.lock();
        if (_selected_light.lock() != selected_light && is_matching_level(selected_light, this))
        {
            sync_room(selected_light);
            _selected_light = light;
        }
    }

    void Level::set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        if (is_matching_level(camera_sink.lock(), this))
        {
            _selected_camera_sink = camera_sink;
        }
    }

    void Level::set_selected_flyby_node(const std::weak_ptr<IFlybyNode>& node)
    {
        if (is_matching_level(node.lock(), this))
        {
            _selected_flyby_node = node;
        }
    }

    void Level::set_selected_sound_source(const std::weak_ptr<ISoundSource>& node)
    {
        if (is_matching_level(node.lock(), this))
        {
            _selected_sound_source = node;
        }
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
        return _platform_and_version.version;
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

            if (auto room = entity->room().lock())
            {
                const auto entity_pos = entity->bounding_box().Center;
                const auto result = room->pick(Vector3(entity_pos.x, entity_pos.y, entity_pos.z), Vector3(0, 1, 0), PickFilter::Geometry | PickFilter::StaticMeshes);
                if (!result.empty())
                {
                    const auto hit = result.front();
                    if (hit.hit)
                    {
                        const auto new_height = hit.position.y - entity->bounding_box().Extents.y;
                        entity->adjust_y(new_height - entity_pos.y);
                    }
                }
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
                _lights.push_back(light_source(static_cast<uint32_t>(_lights.size()), _rooms[i], light, shared_from_this()));
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

            std::vector<std::weak_ptr<IRoom>> in_space_rooms;
            std::vector<std::weak_ptr<IRoom>> in_portal_rooms;

            for (const auto& room : _rooms)
            {
                if (std::shared_ptr<ISector> sector = sector_from_point(*room, point))
                {
                    if (sector && sector->is_portal())
                    {
                        in_portal_rooms.push_back(room);
                    }
                    else
                    {
                        in_space_rooms.push_back(room);
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

            const std::vector<std::weak_ptr<IRoom>> inferred_rooms{ in_space_rooms.empty() ? in_portal_rooms : in_space_rooms };

            const ICameraSink::Type type = is_camera ? ICameraSink::Type::Camera : ICameraSink::Type::Sink;
            auto new_camera_sink = camera_sink_source(i, camera_sink, type, inferred_rooms, relevant_triggers, shared_from_this());
            _token_store += new_camera_sink->on_changed += [this]() { content_changed(); };
            _camera_sinks.push_back(new_camera_sink);

            if (is_camera)
            {
                if (auto room = new_camera_sink->room().lock())
                {
                    room->add_camera_sink(new_camera_sink);
                }
            }
            else
            {
                std::ranges::for_each(inferred_rooms, [&](auto&& r)
                    {
                        if (auto room = r.lock())
                        {
                            room->add_camera_sink(new_camera_sink);
                        }
                    });
            }
        }
    }

    void Level::generate_flybys(const trlevel::ILevel& level, const IFlyby::Source& flyby_source)
    {
        const auto grouped = level.flyby_cameras() |
            std::views::chunk_by([](auto&& l, auto&& r) { return l.sequence == r.sequence; }) |
            std::ranges::to<std::vector<std::vector<trlevel::tr4_flyby_camera>>>();
        for (const auto& flyby : grouped)
        {
            auto new_flyby = flyby_source(flyby, shared_from_this());
            _token_store += new_flyby->on_changed += [this]() { content_changed(); };
            _flybys.push_back(new_flyby);
        }
    }

    void Level::set_show_camera_sinks(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::CameraSinks, show);
        _regenerate_transparency = true;
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

    void Level::initialise(std::shared_ptr<trlevel::ILevel> level,
        std::shared_ptr<IMeshStorage> mesh_storage,
        std::shared_ptr<IModelStorage> model_storage,
        const IItem::EntitySource& entity_source,
        const IItem::AiSource& ai_source,
        const IRoom::Source& room_source,
        const ITrigger::Source& trigger_source,
        const ILight::Source& light_source,
        const ICameraSink::Source& camera_sink_source,
        const ISoundSource::Source& sound_source_source,
        const IFlyby::Source& flyby_source,
        const trlevel::ILevel::LoadCallbacks& callbacks)
    {
        _platform_and_version = level->platform_and_version();
        _floor_data = level->get_floor_data_all();
        _name = level->name();
        _ng = level->trng();
        _pack = level->pack().lock();
        _hash = level->hash();
        _model_storage = model_storage;
        messages::get_settings(_messaging, weak_from_this());

        record_models(*level);
        callbacks.on_progress("Generating rooms");
        generate_rooms(*level, room_source, *mesh_storage);
        callbacks.on_progress("Generating triggers");
        generate_triggers(trigger_source);
        callbacks.on_progress("Generating entities");
        generate_entities(*level, entity_source, ai_source, *model_storage, callbacks);
        callbacks.on_progress("Generating lights");
        generate_lights(*level, light_source);
        callbacks.on_progress("Generating camera/sinks");
        generate_camera_sinks(*level, camera_sink_source);
        callbacks.on_progress("Generating flyby cameras");
        generate_flybys(*level, flyby_source);
        callbacks.on_progress("Generating sound sources");
        generate_sound_sources(*level, sound_source_source);

        callbacks.on_progress("Generating room bounding boxes");
        for (auto& room : _rooms)
        {
            room->update_bounding_box();
        }

        apply_ocb_adjustment();

        callbacks.on_progress("Generating static meshes");
        record_static_meshes();

        callbacks.on_progress("Done");
    }

    void Level::record_static_meshes()
    {
        std::vector<std::weak_ptr<IStaticMesh>> results;
        for (const auto& room : _rooms)
        {
            results.append_range(room->static_meshes());
        }

        uint32_t index = 0;
        for (auto& stat : results)
        {
            if (auto stat_ptr = stat.lock())
            {
                _token_store += stat_ptr->on_changed += [this]() { content_changed(); };
                stat_ptr->set_number(index);
            }
            ++index;
        }
        _static_meshes = results;
    }

    std::vector<std::weak_ptr<IStaticMesh>> Level::static_meshes() const
    {
        return _static_meshes;
    }

    void Level::content_changed()
    {
        _regenerate_transparency = true;
    }

    std::weak_ptr<IStaticMesh> Level::static_mesh(uint32_t index) const
    {
        if (index >= _static_meshes.size())
        {
            return {};
        }
        return _static_meshes[index];
    }

    void Level::add_scriptable(const std::weak_ptr<IScriptable>& scriptable)
    {
        _scriptables.push_back(scriptable);
        if (auto scriptable_ptr = scriptable.lock())
        {
            _token_store += scriptable_ptr->on_changed += [this]() { content_changed(); };
        }
    }

    std::weak_ptr<ISoundStorage> Level::sound_storage() const
    {
        return _sound_storage;
    }

    std::vector<std::weak_ptr<ISoundSource>> Level::sound_sources() const
    {
        return _sound_sources | std::ranges::to<std::vector<std::weak_ptr<ISoundSource>>>();
    }

    void Level::generate_sound_sources(const trlevel::ILevel& level, const ISoundSource::Source& sound_source_source)
    {
        uint32_t count = 0;
        const auto sound_map = level.sound_map();
        const auto details = level.sound_details();
        for (const auto& source : level.sound_sources())
        {
            std::optional<trlevel::tr_x_sound_details> detail;
            if (source.SoundID >= 0 && source.SoundID < sound_map.size())
            {
                const auto index = sound_map[source.SoundID];
                if (index >= 0 && index < details.size())
                {
                    detail = details[index];
                }
            }
            auto sound_source = sound_source_source(count++, source, detail, _platform_and_version.version, shared_from_this());
            _token_store += sound_source->on_changed += [this]() { content_changed(); };
            _sound_sources.push_back(sound_source);
        }
    }

    void Level::set_show_sound_sources(bool show)
    {
        _render_filters = set_flag(_render_filters, RenderFilter::SoundSources, show);
        _regenerate_transparency = true;
    }

    bool Level::show_sound_sources() const
    {
        return has_flag(_render_filters, RenderFilter::SoundSources);
    }

    bool Level::ng_plus() const
    {
        return has_flag(_render_filters, RenderFilter::NgPlus);
    }

    void Level::set_ng_plus(bool show)
    {
        bool currently_active = has_flag(_render_filters, RenderFilter::NgPlus);
        if (currently_active != show)
        {
            _render_filters = set_flag(_render_filters, RenderFilter::NgPlus, show);
            messages::send_ng_plus(_messaging, show);

            if (const auto selected = _selected_item.lock())
            {
                const auto ng = selected->ng_plus();
                if (ng.has_value() && ng != show)
                {
                    const auto alternate = std::ranges::find_if(
                        _entities, [selected](auto&& i)
                        {
                            return i != selected &&
                                i->number() == selected->number() &&
                                i->ng_plus() != selected->ng_plus();
                        });
                    if (alternate != _entities.end())
                    {
                        _selected_item = *alternate;
                        messages::send_select_item(_messaging, _selected_item);
                    }
                }
            }
        }
    }

    bool Level::trng() const
    {
        return _ng;
    }

    std::weak_ptr<trlevel::IPack> Level::pack() const
    {
        return _pack;
    }

    std::string Level::hash() const
    {
        return _hash;
    }

    std::vector<std::weak_ptr<IFlyby>> Level::flybys() const
    {
        return _flybys | std::ranges::to<std::vector<std::weak_ptr<IFlyby>>>();
    }

    void Level::update(float delta)
    {
        if (!_show_animation)
        {
            return;
        }

        for (auto& room : _rooms)
        {
            room->update(delta);
        }
    }

    void Level::set_show_animation(bool show)
    {
        _show_animation = show;
    }

    void Level::receive_message(const Message& message)
    {
        if (auto selected_item = messages::read_select_item(message))
        {
            set_selected_item(selected_item.value());
        }
        else if (auto settings = messages::read_settings(message))
        {
            _map_colours = settings->map_colours;
            on_geometry_colours_changed();
        }
        else if (auto selected_light = messages::read_select_light(message))
        {
            set_selected_light(selected_light.value());
        }
        else if (auto selected_trigger = messages::read_select_trigger(message))
        {
            set_selected_trigger(selected_trigger.value());
        }
        else if (auto selected_room = messages::read_select_room(message))
        {
            set_selected_room(selected_room.value());
        }
        else if (auto selected_flyby_node = messages::read_select_flyby_node(message))
        {
            set_selected_flyby_node(selected_flyby_node.value());
        }
        else if (auto selected_sound_source = messages::read_select_sound_source(message))
        {
            set_selected_sound_source(selected_sound_source.value());
        }
        else if (const auto selected_camera_sink = messages::read_select_camera_sink(message))
        {
            set_selected_camera_sink(selected_camera_sink.value());
        }
        else if (message.type == "get_selected_item")
        {
            messages::reply_to(message, "select_item", _selected_item);
        }
        else if (message.type == "get_selected_room")
        {
            messages::reply_to(message, "select_room", _selected_room);
        }
        else if (message.type == "get_selected_light")
        {
            messages::reply_to(message, "select_light", _selected_light);
        }
        else if (message.type == "get_selected_trigger")
        {
            messages::reply_to(message, "select_trigger", _selected_trigger);
        }
        else if (message.type == "get_selected_camera_sink")
        {
            messages::reply_to(message, "select_camera_sink", _selected_camera_sink);
        }
        else if (message.type == "get_selected_sound_source")
        {
            messages::reply_to(message, "select_sound_source", _selected_sound_source);
        }
        else if (const auto unhide = messages::commands::read_unhide_all(message))
        {
            for (const auto& item : _entities) { item->set_visible(true); };
            for (const auto& trigger : _triggers) { trigger->set_visible(true); }
            for (const auto& light : _lights) { light->set_visible(true); }
            for (const auto& room : _rooms) { room->set_visible(true); }
            for (const auto& camera_sink : _camera_sinks) { camera_sink->set_visible(true); }
            for (const auto& static_mesh : _static_meshes) { if (auto stat = static_mesh.lock()) { stat->set_visible(true); } };
            for (const auto& sound_source : _sound_sources) { sound_source->set_visible(true); };
        }
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

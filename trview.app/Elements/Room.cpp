#include "Room.h"
#include <trview.app/Geometry/MeshVertex.h>
#include <trview.app/Camera/ICamera.h>
#include <trview.app/Elements/ILevel.h>
#include <trview.common/Algorithms.h>
#include <format>
#include <trview.common/Logs/Activity.h>
#include <ranges>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const Color Unmatched_Colour{ 0, 0.75f, 0.75f };

        const Color Selected_Colour{ 1, 1, 1 };
        const Color SelectedWater_Colour{ 0.4f, 0.9f, 1.0f };
        const Color NotSelected_Colour{ 0.4f, 0.4f, 0.4f };
        const Color NotSelectedWater_Colour{ 0.16f, 0.36f, 0.4f };

        Color room_colour(bool water, Room::SelectionMode selected)
        {
            return selected == Room::SelectionMode::Selected
                ? (water ? SelectedWater_Colour : Selected_Colour)
                : (water ? NotSelectedWater_Colour : NotSelected_Colour);
        }

        Color get_unmatched_colour(const RoomInfo info, const ISector& sector)
        {
            uint32_t x = (sector.x() + info.x / 1024) % 2;
            uint32_t z = info.z / 1024 + sector.z();
            return (x + z) % 2 ? Unmatched_Colour : Unmatched_Colour + Color(0, 0.05f, 0.05f);
        }

        // Follow portals until we hit a non portal sector
        void follow_portal(ISector::Portal& portal, const std::shared_ptr<ILevel>& level, const IRoom& room, uint16_t target_room, int x, int z)
        {
            const auto other_room = level->room(target_room).lock();
            const auto diff = (room.position() - other_room->position()) + Vector3(static_cast<float>(x), 0, static_cast<float>(z));
            const int other_id = static_cast<int>(diff.x * other_room->num_z_sectors() + diff.z);
            const auto sectors = other_room->sectors();
            if (other_id >= 0 && other_id < std::ssize(sectors))
            {
                portal.target = sectors[other_id];
                portal.offset += Vector3(static_cast<float>(x), 0, static_cast<float>(z)) - diff;
                if (portal.target->is_portal())
                {
                    follow_portal(portal, level, *other_room, portal.target->portals()[0], portal.target->x(), portal.target->z());
                }
            }
        }

        enum class VerticalPortalDirection
        {
            Above,
            Below
        };

        std::optional<VerticalPortalInfo> vertical_sector(const std::shared_ptr<ISector>& sector, VerticalPortalDirection direction)
        {
            if ((sector->room_above() == 0xff && direction == VerticalPortalDirection::Above) ||
                (sector->room_below() == 0xff && direction == VerticalPortalDirection::Below))
            {
                return std::nullopt;
            }

            const auto room = sector->room().lock();
            if (!room)
            {
                return std::nullopt;
            }

            const auto level = room->level().lock();
            if (!level)
            {
                return std::nullopt;
            }

            const auto other_room = level->room(direction == VerticalPortalDirection::Above ? sector->room_above() : sector->room_below()).lock();
            if (!other_room)
            {
                return std::nullopt;
            }

            const int32_t x = sector->x();
            const int32_t z = sector->z();
            const Vector3 sector_position = Vector3(static_cast<float>(x), 0, static_cast<float>(z));

            const auto diff = (room->position() - other_room->position()) + sector_position;
            if (auto other_sector = other_room->sector(static_cast<int32_t>(diff.x), static_cast<int32_t>(diff.z)).lock())
            {
                return VerticalPortalInfo
                {
                    .sector = other_sector,
                    .offset = sector_position - diff,
                    .room = other_room
                };
            }

            return std::nullopt;
        }
    }

    std::optional<VerticalPortalInfo> sector_above(const std::shared_ptr<ISector>& sector)
    {
        return vertical_sector(sector, VerticalPortalDirection::Above);
    }

    std::optional<VerticalPortalInfo> sector_below(const std::shared_ptr<ISector>& sector)
    {
        return vertical_sector(sector, VerticalPortalDirection::Below);
    }

    IRoom::~IRoom()
    {
    }

    Room::Room(const trlevel::tr3_room& room,
        const IMesh::Source& mesh_source,
        std::shared_ptr<ILevelTextureStorage> texture_storage,
        uint32_t index,
        const std::weak_ptr<ILevel>& parent_level,
        const graphics::ISamplerState::Source& sampler_source)
        : _info { room.info.x, 0, room.info.z, room.info.yBottom, room.info.yTop }, 
        _alternate_room(room.alternate_room),
        _alternate_group(room.alternate_group),
        _num_x_sectors(room.num_x_sectors),
        _num_z_sectors(room.num_z_sectors),
        _index(index),
        _flags(room.flags),
        _level(parent_level),
        _texture_storage(texture_storage),
        _mesh_source(mesh_source),
        _ambient(room.colour),
        _ambient_intensity_1(room.ambient_intensity_1),
        _ambient_intensity_2(room.ambient_intensity_2),
        _light_mode(room.light_mode),
        _water_scheme(room.water_scheme)
    {
        _ambient.a = 1.0f;

        // Can only determine HasAlternate or normal at this point. After all rooms have been loaded,
        // the level can fix up the rooms so that they know if they are alternates of another room
        // (IsAlternate).
        _alternate_mode = (room.alternate_room != -1 || !equals_any(room.alternate_group, 0, 0xff)) ? AlternateMode::HasAlternate : AlternateMode::None;

        _room_offset = Matrix::CreateTranslation(room.info.x / trlevel::Scale_X, 0, room.info.z / trlevel::Scale_Z);
        _inverted_room_offset = _room_offset.Invert();

        if (auto parent = _level.lock())
        {
            _token_store += parent->on_geometry_colours_changed += [&]() { _all_geometry_meshes.clear(); };
        }

        _geometry_sampler_state = sampler_source(graphics::ISamplerState::AddressMode::Wrap);
        _sampler_state = sampler_source(graphics::ISamplerState::AddressMode::Clamp);
    }

    void Room::initialise(const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage,
        const IStaticMesh::MeshSource& static_mesh_mesh_source, const IStaticMesh::PositionSource& static_mesh_position_source,
        const ISector::Source& sector_source, uint32_t sector_base_index, const Activity& activity)
    {
        generate_sectors(level, room, sector_source, sector_base_index);
        generate_geometry(_mesh_source, room);
        generate_adjacency();
        generate_static_meshes(_mesh_source, level, room, mesh_storage, static_mesh_mesh_source, static_mesh_position_source, activity);
    }

    RoomInfo Room::info() const
    {
        return _info;
    }

    std::set<uint16_t> Room::neighbours() const
    {
        return _neighbours;
    }

    std::vector<PickResult> Room::pick(const Vector3& position, const Vector3& direction, PickFilter filters) const
    {
        using namespace DirectX::TriangleTests;

        // Test against bounding box for the room first, to avoid more expensive mesh-ray intersection
        float box_distance = 0;
        if (!_bounding_box.Intersects(position, direction, box_distance))
        {
            return {};
        }

        std::vector<PickResult> pick_results;

        if (has_flag(filters, PickFilter::Entities))
        {
            // Pick against the entity geometry:
            for (const auto& entity : _entities)
            {
                auto entity_ptr = entity.lock();
                if (!entity_ptr || !entity_ptr->visible())
                {
                    continue;
                }

                const auto ng = entity_ptr->ng_plus();
                if (!ng.has_value() || ng.value() == has_flag(filters, PickFilter::NgPlus))
                {
                    auto entity_result = entity_ptr->pick(position, direction);
                    if (entity_result.hit)
                    {
                        pick_results.push_back(entity_result);
                    }
                }
            }
        }

        if (has_flag(filters, PickFilter::Lights))
        {
            for (const auto& light : _lights)
            {
                auto light_ptr = light.lock();
                if (!light_ptr || !light_ptr->visible())
                {
                    continue;
                }

                auto light_result = light_ptr->pick(position, direction);
                if (light_result.hit)
                {
                    pick_results.push_back(light_result);
                }
            }
        }

        if (has_flag(filters, PickFilter::CameraSinks))
        {
            for (const auto& camera_sink : _camera_sinks)
            {
                auto camera_sink_ptr = camera_sink.lock();
                if (!camera_sink_ptr || !camera_sink_ptr->visible())
                {
                    continue;
                }

                auto camera_sink_result = camera_sink_ptr->pick(position, direction);
                if (camera_sink_result.hit)
                {
                    pick_results.push_back(camera_sink_result);
                }
            }
        }

        if (has_flag(filters, PickFilter::Triggers))
        {
            for (const auto& trigger_pair : _triggers)
            {
                auto trigger = trigger_pair.second.lock();
                if (!trigger || !trigger->visible())
                {
                    continue;
                }

                auto trigger_result = trigger->pick(position, direction);
                if (trigger_result.hit)
                {
                    pick_results.push_back(trigger_result);
                }
            }
        }

        if (has_flag(filters, PickFilter::StaticMeshes) && !has_flag(filters, PickFilter::AllGeometry))
        {
            for (const auto& static_mesh : _static_meshes)
            {
                if (!static_mesh->visible())
                {
                    continue;
                }

                PickResult static_mesh_result = static_mesh->pick(position, direction);
                if (static_mesh_result.hit)
                {
                    static_mesh_result.type = PickResult::Type::StaticMesh;
                    static_mesh_result.static_mesh = static_mesh;
                    pick_results.push_back(static_mesh_result);
                }
            }
        }

        if (has_flag(filters, PickFilter::AllGeometry))
        {
            auto room_offset = Matrix::CreateTranslation(-_info.x / trlevel::Scale_X, 0, -_info.z / trlevel::Scale_Z);
            for (const auto& mesh : _all_geometry_meshes)
            {
                PickResult all_geometry_result = mesh.second->pick(Vector3::Transform(position, room_offset), direction);
                if (all_geometry_result.hit)
                {
                    add_centroid_to_pick(*mesh.second, all_geometry_result);
                    pick_results.push_back(all_geometry_result);
                }
            }
        }
        else if (has_flag(filters, PickFilter::Geometry))
        {
            // Pick against the room geometry:
            auto room_offset = Matrix::CreateTranslation(-_info.x / trlevel::Scale_X, 0, -_info.z / trlevel::Scale_Z);
            PickResult geometry_result = _mesh->pick(Vector3::Transform(position, room_offset), direction);
            if (geometry_result.hit)
            {
                add_centroid_to_pick(*_mesh, geometry_result);
                pick_results.push_back(geometry_result);
            }
        }

        std::sort(pick_results.begin(), pick_results.end(),
            [](const auto& l, const auto& r) { return l.distance < r.distance; });
        return pick_results;
    }

    void Room::render(const ICamera& camera, SelectionMode selected, RenderFilter render_filter, const std::unordered_set<uint32_t>& visible_rooms)
    {
        Color colour = room_colour(water() && has_flag(render_filter, RenderFilter::Water), selected);

        if (has_flag(render_filter, RenderFilter::Rooms))
        {
            if (has_flag(render_filter, RenderFilter::AllGeometry))
            {
                if (_all_geometry_meshes.empty())
                {
                    generate_all_geometry_mesh(_mesh_source);
                }

                _geometry_sampler_state->apply();
                for (const auto& mesh : _all_geometry_meshes)
                {
                    if (mesh.first == _index || (visible_rooms.find(mesh.first) == visible_rooms.end() || _index < mesh.first))
                    {
                        mesh.second->render(_room_offset * camera.view_projection(), colour, 1.0f, Vector3::Zero, has_flag(render_filter, RenderFilter::AllGeometry));
                    }
                }
            }
            else
            {
                _sampler_state->apply();
                _mesh->render(_room_offset * camera.view_projection(), colour, 1.0f, Vector3::Zero, false, !has_flag(render_filter, RenderFilter::Lighting));
                for (const auto& mesh : _static_meshes)
                {
                    if (mesh->visible())
                    {
                        mesh->render(camera, colour);
                    }
                }
            }
        }

        render_contained(camera, colour, render_filter);
    }

    void Room::render_bounding_boxes(const ICamera& camera)
    {
        for (const auto& mesh : _static_meshes)
        {
            mesh->render_bounding_box(camera, Colour::White);
        }
    }

    void Room::render_lights(const ICamera& camera, const std::weak_ptr<ILight>& selected_light)
    {
        const auto selected = selected_light.lock();
        for (const auto& light : _lights)
        {
            if (auto light_ptr = light.lock())
            {
                light_ptr->render(camera, Colour::White);
                if (light_ptr == selected)
                {
                    light_ptr->render_direction(camera);
                }
            }
        }
    }

    void Room::render_camera_sinks(const ICamera& camera)
    {
        for (const auto& camera_sink : _camera_sinks)
        {
            if (auto camera_sink_ptr = camera_sink.lock())
            {
                camera_sink_ptr->render(camera, Colour::White);
            }
        }
    }

    void Room::render_contained(const ICamera& camera, SelectionMode selected, RenderFilter render_filter)
    {
        Color colour = room_colour(water() && has_flag(render_filter, RenderFilter::Water), selected);
        render_contained(camera, colour, render_filter);
    }

    void Room::render_contained(const ICamera& camera, const Color& colour, RenderFilter render_filter)
    {
        if (!has_flag(render_filter, RenderFilter::Entities))
        {
            return;
        }

        _sampler_state->apply();
        for (const auto& entity : _entities)
        {
            if (auto entity_ptr = entity.lock())
            {
                const auto ng = entity_ptr->ng_plus();
                if (!ng.has_value() || ng.value() == has_flag(render_filter, RenderFilter::NgPlus))
                {
                    entity_ptr->render(camera, colour);
                }
            }
        }
    }

    void Room::generate_static_meshes(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage,
        const IStaticMesh::MeshSource& static_mesh_mesh_source, const IStaticMesh::PositionSource& static_mesh_position_source, const Activity& activity)
    {
        for (uint32_t i = 0; i < room.static_meshes.size(); ++i)
        {
            auto room_mesh = room.static_meshes[i];
            auto level_static_mesh = level.get_static_mesh(room_mesh.mesh_id);
            if (!level_static_mesh.has_value())
            {
                activity.log(trview::LogMessage::Status::Error, std::format("Static Mesh {} was requested but not found", room_mesh.mesh_id));
                continue;
            }
            _static_meshes.push_back(static_mesh_mesh_source(room_mesh, level_static_mesh.value(), mesh_storage.mesh(level_static_mesh.value().Mesh), shared_from_this(), _level));
        }

        // Also read the room sprites - they're similar enough for now.
        for (const auto& room_sprite : room.data.sprites)
        {
            Matrix scale;
            Vector3 offset;
            auto sprite_mesh = create_sprite_mesh(mesh_source, level.get_sprite_texture(room_sprite.texture), scale, offset, SpriteOffsetMode::RoomSprite);

            auto vertex = room.data.vertices[room_sprite.vertex].vertex;
            auto pos = Vector3(vertex.x / trlevel::Scale_X, vertex.y / trlevel::Scale_Y, vertex.z / trlevel::Scale_Z);
            pos = Vector3::Transform(pos, _room_offset) + offset;
            _static_meshes.push_back(static_mesh_position_source(room_sprite, pos, scale, sprite_mesh, shared_from_this(), _level));
        }
    }

    namespace
    {
        /// Check if the triangle points appear in the position source.
        /// @param tri The triangle points.
        /// @param source The point list.
        bool triangle_contained(const std::vector<Vector3>& tri, const std::vector<Vector3>& source)
        {
            auto v0 = std::find(source.begin(), source.end(), tri[0]);
            auto v1 = std::find(source.begin(), source.end(), tri[1]);
            auto v2 = std::find(source.begin(), source.end(), tri[2]);
            return v0 != source.end() && v1 != source.end() && v2 != source.end();
        }

        /// Check if the triangle points appear in the position source.
        /// @param tri The triangle points.
        /// @param source The point list.
        bool triangle_contained(const ISector::Triangle& tri, const std::vector<Vector3>& source)
        {
            return triangle_contained(std::vector<Vector3>{ tri.v0, tri.v1, tri.v2 }, source);
        }
    }

    void Room::generate_geometry(const IMesh::Source& mesh_source, const trlevel::tr3_room& room)
    {
        std::vector<Triangle> triangles;
        process_textured_rectangles(room.data.rectangles, room.data.vertices, *_texture_storage, triangles, false);
        process_textured_triangles(room.data.triangles, room.data.vertices, *_texture_storage, triangles, false);
        process_collision_transparency(triangles);

        _mesh = mesh_source(triangles);

        // Generate the bounding box based on the room dimensions.
        update_bounding_box();
    }

    void Room::generate_adjacency()
    {
        _neighbours.clear(); 
        std::for_each(_sectors.begin(), _sectors.end(),
            [&] (const auto& sector)
        {
            const std::set<std::uint16_t> n = sector->neighbours(); 
            _neighbours.insert(n.begin(), n.end());
        });
    }

    void Room::add_entity(const std::weak_ptr<IItem>& entity)
    {
        _entities.push_back(entity);
    }

    void Room::add_trigger(const std::weak_ptr<ITrigger>& trigger)
    {
        auto trigger_ptr = trigger.lock();
        if (!trigger_ptr)
        {
            return;
        }
        trigger_ptr->on_changed += on_changed;
        _triggers.insert({ trigger_ptr->sector_id(), trigger });
    }

    void Room::add_light(const std::weak_ptr<ILight>& light)
    {
        _lights.push_back(light);

        if (auto light_ptr = light.lock())
        {
            light_ptr->on_changed += on_changed;

            // Place suns in the middle of the room instead of 9 million units away.
            // Only do this if the light position is massive - don't change Tomb Editor placed suns.
            // Stack them in a line so they don't overlap each other.
            if (light_ptr->type() == trlevel::LightType::Sun && std::abs(light_ptr->position().x) > 9000)
            {
                uint32_t suns = 0;
                for (const auto& other_light : _lights)
                {
                    auto other_light_ptr = other_light.lock();
                    if (other_light_ptr->type() == trlevel::LightType::Sun && other_light_ptr != light_ptr)
                    {
                        ++suns;
                    }
                }
                light_ptr->set_position(centre() + Vector3(0, 0, 0.25f * suns));
            }
        }
    }

    void Room::add_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink)
    {
        _camera_sinks.push_back(camera_sink);
    }

    void Room::generate_sectors(const trlevel::ILevel& level, const trlevel::tr3_room& room, const ISector::Source& sector_source, uint32_t sector_base_index)
    {
        for (auto i = 0u; i < room.sector_list.size(); ++i)
        {
            const trlevel::tr_room_sector &sector = room.sector_list[i];
            _sectors.push_back(sector_source(level, room, sector, i, shared_from_this(), sector_base_index + i));
        }
    }

    void Room::get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, RenderFilter render_filter)
    {
        Color colour = room_colour(water() && has_flag(render_filter, RenderFilter::Water), selected);

        if (has_flag(render_filter, RenderFilter::Rooms))
        {
            if (!has_flag(render_filter, RenderFilter::AllGeometry))
            {
                for (const auto& triangle : _mesh->transparent_triangles())
                {
                    transparency.add(triangle.transform(_room_offset, colour, !has_flag(render_filter, RenderFilter::Lighting)));
                }

                for (const auto& static_mesh : _static_meshes)
                {
                    if (static_mesh->visible())
                    {
                        static_mesh->get_transparent_triangles(transparency, camera, colour);
                    }
                }
            }
        }

        if (has_flag(render_filter, RenderFilter::Triggers))
        {
            for (const auto& trigger_pair : _triggers)
            {
                if (auto trigger = trigger_pair.second.lock())
                {
                    trigger->get_transparent_triangles(transparency, camera, trigger->colour());
                }
            }
        }

        get_contained_transparent_triangles(transparency, camera, colour, render_filter);
    }

    void Room::get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, RenderFilter render_filter)
    {
        Color colour = room_colour(water() && has_flag(render_filter, RenderFilter::Water), selected);
        get_contained_transparent_triangles(transparency, camera, colour, render_filter);
    }

    void Room::get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const Color& colour, RenderFilter render_filter)
    {
        if (!has_flag(render_filter, RenderFilter::Entities))
        {
            return;
        }

        for (const auto& entity : _entities)
        {
            if (auto entity_ptr = entity.lock())
            {
                const auto ng = entity_ptr->ng_plus();
                if (!ng.has_value() || ng.value() == has_flag(render_filter, RenderFilter::NgPlus))
                {
                    entity_ptr->get_transparent_triangles(transparency, camera, colour);
                }
            }
        }
    }

    // Determines the alternate state of the room.
    Room::AlternateMode Room::alternate_mode() const
    {
        return _alternate_mode;
    }

    // Gets the room number of the room that is the alternate to this room.
    // If this room does not have an alternate this will be -1.
    // Returns: The room number of the alternate room.
    int16_t Room::alternate_room() const
    {
        return _alternate_room;
    }

    int16_t Room::alternate_group() const
    {
        return _alternate_group;
    }

    // Set this room to be the alternate room of the room specified.
    // This will change the alternate_mode of this room to IsAlternate.
    // number: The room number.
    void Room::set_is_alternate(int16_t number)
    {
        _alternate_room = number;
        _alternate_mode = AlternateMode::IsAlternate;
    }

    Vector3 Room::centre() const
    {
        return Vector3(_info.x / trlevel::Scale_X + _num_x_sectors / 2.f,
                 _info.yTop / trlevel::Scale_Y + (_info.yBottom - _info.yTop) / trlevel::Scale_Y / 2.0f,
                 (_info.z / trlevel::Scale_Z) + _num_z_sectors / 2.f);
    }

    DirectX::BoundingBox Room::bounding_box() const
    {
        return _bounding_box;
    }

    void Room::generate_trigger_geometry()
    {
        for (auto& trigger_iter : _triggers)
        {
            auto trigger = trigger_iter.second.lock();
            if (!trigger)
            {
                continue;
            }

            auto sector = _sectors[trigger->sector_id()];
            auto y_bottom = sector->corners();

            // Figure out if we should make the walls based on adjacent triggers.
            bool pos_x = true, neg_x = true, pos_z = true, neg_z = true;

            if (auto other = get_trigger_sector(*trigger, 1, 0))
            {
                auto corners = other->corners();
                if (y_bottom[3] == corners[1] && y_bottom[2] == corners[0])
                {
                    pos_x = false;
                }
            }

            if (auto other = get_trigger_sector(*trigger, -1, 0))
            {
                auto corners = other->corners();
                if (y_bottom[1] == corners[3] && y_bottom[0] == corners[2])
                {
                    neg_x = false;
                }
            }

            if (auto other = get_trigger_sector(*trigger, 0, -1))
            {
                auto corners = other->corners();
                if (y_bottom[2] == corners[3] && y_bottom[0] == corners[1])
                {
                    neg_z = false;
                }
            }

           if (auto other = get_trigger_sector(*trigger, 0, 1))
           {
                auto corners = other->corners();
                if (y_bottom[3] == corners[2] && y_bottom[1] == corners[0])
                {
                    pos_z = false;
                }
            }

            // Calculate the X/Z position.
            const float x = _info.x / trlevel::Scale_X + trigger->x() + 0.5f;
            const float z = _info.z / trlevel::Scale_Z + trigger->z() + 0.5f;
            const float height = 0.25f;

            std::array<float, 4> y_top = { 0,0,0,0 };
            for (int i = 0; i < 4; ++i)
            {
                y_top[i] = y_bottom[i] - height;
            }

            const std::array<Vector3, 4> t
            {
                Vector3(x - 0.5f, y_top[0], z - 0.5f),
                Vector3(x - 0.5f, y_top[1], z + 0.5f),
                Vector3(x + 0.5f, y_top[2], z - 0.5f),
                Vector3(x + 0.5f, y_top[3], z + 0.5f)
            };
            const std::array<Vector3, 4> b
            {
                Vector3(x - 0.5f, y_bottom[0], z - 0.5f),
                Vector3(x - 0.5f, y_bottom[1], z + 0.5f),
                Vector3(x + 0.5f, y_bottom[2], z - 0.5f),
                Vector3(x + 0.5f, y_bottom[3], z + 0.5f)
            };

            std::vector<Triangle> triangles;
            const Color colour = ITrigger::Trigger_Colour;

            const auto add_tri = [&triangles, &colour](const Vector3& v0, const Vector3& v1, const Vector3& v2)
            {
                const auto normal = (v2 - v1).Cross(v1 - v0);
                triangles.push_back(
                    Triangle
                    {
                        .colours = { colour, colour, colour },
                        .normals { normal, normal, normal },
                        .texture_mode = Triangle::TextureMode::Untextured,
                        .transparency_mode = Triangle::TransparencyMode::Normal,
                        .vertices = { v0, v1, v2 }
                    });
            };

            // + Y
            if (sector->triangulation() == TriangulationDirection::NwSe)
            {
                add_tri(t[3], t[2], t[1]);
                add_tri(t[0], t[1], t[2]);
            }
            else
            {
                add_tri(t[3], t[2], t[0]);
                add_tri(t[0], t[1], t[3]);
            }

            if (pos_x)
            {
                // + X
                add_tri(t[2], t[3], b[3]);
                add_tri(t[2], b[3], b[2]);
            }

            if (neg_x)
            {
                // - X
                add_tri(t[1], t[0], b[0]);
                add_tri(t[1], b[0], b[1]);
            }

            if (pos_z)
            {
                // + Z
                add_tri(t[3], t[1], b[1]);
                add_tri(t[3], b[1], b[3]);
            }

            if (neg_z)
            {
                // - Z
                add_tri(t[0], t[2], b[2]);
                add_tri(t[0], b[2], b[0]);
            }

            trigger->set_triangles(triangles);

            float centre_y = std::accumulate(y_top.begin(), y_top.end(), std::accumulate(y_bottom.begin(), y_bottom.end(), 0.0f)) / 8.0f;

            trigger->set_position(Vector3(x, centre_y, z));
        }
    }

    uint32_t Room::get_sector_id(int32_t x, int32_t z) const
    {
        return x * _num_z_sectors + z;
    }

    ISector* Room::get_trigger_sector(const ITrigger& trigger, int32_t dx, int32_t dz)
    {
        const int16_t x = static_cast<int16_t>(trigger.x() + dx);
        const int16_t z = static_cast<int16_t>(trigger.z() + dz);
        auto sector_id = get_sector_id(x, z);
        auto next_trigger = _triggers.find(sector_id);
        if (next_trigger != _triggers.end())
        {
            return _sectors[sector_id].get();
        }

        if (sector_id >= _sectors.size())
        {
            return nullptr;
        }

        // Check if this sector is a portal.
        const auto sector = _sectors[sector_id];
        if (!has_flag(sector->flags(), SectorFlag::Portal))
        {
            return nullptr;
        }

        auto level = _level.lock();
        if (!level)
        {
            return nullptr;
        }

        for (auto p : sector->portals())
        {
            auto room = level->room(p).lock();
            if (!room)
            {
                continue;
            }

            // Get the world position of the target sector.
            auto world_x = (_info.x / trlevel::Scale_X) + x;
            auto world_z = (_info.z / trlevel::Scale_Z) + z;

            // Convert the world position into the space of the other room.
            auto other_x = static_cast<int32_t>(world_x - (room->info().x / trlevel::Scale_X));
            auto other_z = static_cast<int32_t>(world_z - (room->info().z / trlevel::Scale_Z));

            auto other_trigger = room->trigger_at(other_x, other_z).lock();
            if (other_trigger)
            {
                return room->sectors()[other_trigger->sector_id()].get();
            }
        }
        return nullptr;
    }

    namespace
    {
        void add_triangle(const ISector::Triangle& tri, std::vector<Triangle>& triangles, const Color& color)
        {
            triangles.push_back(
                {
                    .colours = { color, color, color },
                    .frames = { { .uvs = { tri.uv0, tri.uv1, tri.uv2 } } },
                    .normals = { Vector3::Down, Vector3::Down, Vector3::Down },
                    .vertices = { tri.v0, tri.v1, tri.v2 }
                });
        }
    }

    void Room::process_collision_transparency(std::vector<Triangle>& triangles)
    {
        for (auto& triangle : triangles)
        {
            if (triangle.transparency_mode == Triangle::TransparencyMode::None)
            {
                continue;
            }

            for (const auto& sector : _sectors)
            {
                if (!sector->is_floor())
                {
                    continue;
                }

                const float x = sector->x() + 0.5f;
                const float z = sector->z() + 0.5f;
                const auto corners = sector->corners();

                if (triangle_contained(
                    { triangle.vertices, triangle.vertices + 3 },
                    { { x + 0.5f, corners[2], z - 0.5f },
                      { x - 0.5f, corners[1], z + 0.5f },
                      { x + 0.5f, corners[3], z + 0.5f },
                      { x - 0.5f, corners[0], z - 0.5f } }))
                {
                    triangle.collision_mode = Triangle::CollisionMode::Enabled;
                    // A triangle can only match in one sector, so stop after adding it once.
                    break;
                }
            }
        }
    }

    uint32_t Room::number() const
    {
        return _index;
    }

    void Room::update_bounding_box()
    {
        using namespace DirectX;
        // Get the extents of the room from the information - doesn't take into account any entities.
        const auto extents = Vector3(_num_x_sectors, (_info.yBottom - _info.yTop) / trlevel::Scale_Y, _num_z_sectors) * 0.5f;
        _bounding_box = BoundingBox(centre(), extents);
        // Merge all entity bounding boxes with the room bounding box.
        for (const auto& entity : _entities)
        {
            if (auto entity_ptr = entity.lock())
            {
                BoundingBox::CreateMerged(_bounding_box, _bounding_box, entity_ptr->bounding_box());
            }
        }

        for (const auto& camera_sink : _camera_sinks)
        {
            if (auto camera_sink_ptr = camera_sink.lock())
            {
                BoundingBox::CreateMerged(_bounding_box, _bounding_box, camera_sink_ptr->bounding_box());
            }
        }
    }

    bool Room::outside() const
    {
        return _flags & 0x8;
    }

    bool Room::water() const
    {
        return _flags & 0x1;
    }

    bool Room::quicksand() const
    {
        if (auto level = _level.lock())
        {
            if (level->version() != trlevel::LevelVersion::Tomb3)
            {
                return false;
            }
        }
        return (_flags & 0x80);
    }

    std::weak_ptr<ISector> Room::sector(int32_t x, int32_t z) const
    {
        const auto id = get_sector_id(x, z);
        if (id < _sectors.size())
        {
            return _sectors[id];
        }
        return {};
    }

    Vector3 Room::sector_centroid(const std::weak_ptr<ISector>& sector) const
    {
        auto sector_ptr = sector.lock();
        if (!sector_ptr)
        {
            return Vector3::Zero;
        }

        const auto corners = sector_ptr->corners();
        const Vector3 centroid =
        {
            static_cast<float>(sector_ptr->x()) + 0.5f,
            std::accumulate(corners.begin(), corners.end(), 0.0f) / 4.0f,
            static_cast<float>(sector_ptr->z()) + 0.5f
        };
        return Vector3::Transform(centroid, _room_offset);
    }

    std::vector<std::shared_ptr<ISector>> Room::sectors() const
    {
        return _sectors; 
    }

    std::weak_ptr<ITrigger> Room::trigger_at(int32_t x, int32_t z) const
    {
        auto sector_id = get_sector_id(x, z);
        auto found = _triggers.find(sector_id);
        if (found != _triggers.end())
        {
            return found->second;
        }
        return {};
    }

    uint16_t Room::num_x_sectors() const
    {
        return _num_x_sectors; 
    }

    // Returns the number of z sectors in the room 
    uint16_t Room::num_z_sectors() const 
    {
        return _num_z_sectors; 
    }

    void Room::add_centroid_to_pick(const IMesh& mesh, PickResult& geometry_result) const
    {
        // Transform the position back in to world space. Also mark it as a room pick result.
        geometry_result.type = PickResult::Type::Room;
        geometry_result.room = std::const_pointer_cast<IRoom>(shared_from_this());
        geometry_result.position = Vector3::Transform(geometry_result.position, _room_offset);

        const auto& tri = geometry_result.triangle;
        if (tri.normal().y < 0)
        {
            Vector3 centroid = { std::floor(geometry_result.position.x) + 0.5f, geometry_result.position.y, std::floor(geometry_result.position.z) + 0.5f };
            Vector3 ray_direction = { 0, -tri.normal().y, 0 };

            centroid = Vector3::Transform(centroid, _inverted_room_offset);
            ray_direction.Normalize();
            PickResult centroid_hit = mesh.pick(centroid - ray_direction * 0.5f, ray_direction);
            geometry_result.centroid = centroid_hit.hit ? Vector3::Transform(centroid_hit.position, _room_offset) : geometry_result.position;
            geometry_result.triangle = centroid_hit.hit ? centroid_hit.triangle : geometry_result.triangle;
        }
        else
        {
            geometry_result.centroid = geometry_result.position;
        }
    }

    bool Room::flag(Flag flag) const
    {
        return _flags & static_cast<uint16_t>(flag);
    }

    uint16_t Room::flags() const
    {
        return _flags;
    }
    
    float Room::y_bottom() const
    {
        return _info.yBottom / trlevel::Scale_Y;
    }

    float Room::y_top() const
    {
        return _info.yTop / trlevel::Scale_Y;
    }

    void Room::generate_sector_triangles()
    {
        for (const auto& sector : _sectors)
        {
            sector->generate_triangles();
        }
    }

    void Room::generate_all_geometry_mesh(const IMesh::Source& mesh_source)
    {
        // TODO: Split into meshes for the main room and then for adjacent rooms. If the adjacent room is being rendered
        // then only one room needs to render that part. This can be decided based on which room has the lower room number.
        auto level = _level.lock();
        if (!level)
        {
            return;
        }

        const auto colours = level->map_colours();

        auto tri_colour = [&](const ISector::Triangle& tri) -> Colour
        {
            if (has_flag(tri.type, SectorFlag::Death))
            {
                return colours.colour(SectorFlag::Death);
            }
            else if (has_any_flag(tri.type, SectorFlag::ClimbableSouth, SectorFlag::ClimbableWest, SectorFlag::ClimbableEast, SectorFlag::ClimbableNorth))
            {
                return colours.colour(tri.type & SectorFlag::Climbable);
            }
            else if (has_flag(tri.type, SectorFlag::Wall))
            {
                return colours.colour(MapColours::Special::GeometryWall);
            }
            else if (has_flag(tri.type, SectorFlag::MonkeySwing))
            {
                return colours.colour(SectorFlag::MonkeySwing);
            }
            return colours.colour(MapColours::Special::Default);
        };

        struct MeshPart
        {
            std::vector<Triangle> triangles;
        };

        std::unordered_map<uint32_t, MeshPart> mesh_parts;
        std::size_t base = 0u;
        for (const auto& sector : _sectors)
        {
            const auto tris = sector->triangles();
            for (std::size_t i = 0; i < tris.size(); ++i)
            {
                const auto& tri = tris[i];
                auto& part = mesh_parts[_all_geometry_sector_rooms[base + i]];
                auto colour = tri_colour(tri);
                colour.a = 1.0f;
                add_triangle(tri, part.triangles, colour);
            }
            base += tris.size();
        }

        for (const auto& parts : mesh_parts)
        {
            _all_geometry_meshes[parts.first] = mesh_source(parts.second.triangles);
        }
    }

    ISector::Portal Room::sector_portal(int x1, int z1, int x2, int z2) const
    {
        ISector::Portal portal;

        auto level = _level.lock();
        if (!level)
        {
            return portal;
        }

        auto sector = _sectors[get_sector_id(x1, z1)];
        if (auto above = sector_above(sector))
        {
            portal.sector_above = above->sector;
            portal.above_offset = above->offset;
            portal.room_above = above->room;
        }

        if (auto below = sector_below(sector))
        {
            portal.sector_below = below->sector;
            portal.below_offset = below->offset;
            portal.room_below = below->room;
        }

        if (x2 >= _num_x_sectors || x2 < 0 || z2 >= _num_z_sectors || z2 < 0)
        {
            return portal;
        }

        const auto id = get_sector_id(x2, z2);
        portal.direct = _sectors[id];
        portal.direct_room = std::const_pointer_cast<IRoom>(shared_from_this());

        if (x1 == x2 && z1 == z2)
        {
            return portal;
        }

        if (!portal.direct->is_portal() || sector->is_wall())
        {
            return portal;
        }

        const auto box = bounding_box();
        for (const auto& p : portal.direct->portals())
        {
            if (const auto target_room = level->room(p).lock())
            {
                const auto target_room_box = target_room->bounding_box();
                const float top = box.Center.y - box.Extents.y;
                const auto diff = (position() - target_room->position()) + Vector3(static_cast<float>(x2), 0, static_cast<float>(z2));
                const int other_id = static_cast<int>(diff.x * target_room->num_z_sectors() + diff.z);
                const auto target_sectors = target_room->sectors();
                if (other_id >= 0 && other_id < std::ssize(target_sectors))
                {
                    const auto target_sector = target_sectors[other_id];
                    const float target_bottom = target_sector->corner(ISector::Corner::NE).y;
                    const float target_top = target_sector->ceiling(ISector::Corner::NE).y;
                    const bool above = target_top < top && target_bottom <= top;

                    // Only include rooms that overlap and aren't completely above.
                    if (box.Intersects(target_room_box) && !above)
                    {
                        follow_portal(portal, level, *this, portal.direct->portals()[0], x2, z2);
                        return portal;
                    }
                }
            }
        }
        return portal;
    }

    void Room::set_sector_triangle_rooms(const std::vector<uint32_t>& triangles)
    {
        _all_geometry_sector_rooms = triangles;
    }

    Vector3 Room::position() const
    {
        return Vector3(_info.x / trlevel::Scale_X, 0, _info.z / trlevel::Scale_Z);
    }

    bool Room::visible() const
    {
        return _visible;
    }

    void Room::set_visible(bool visible)
    {
        _visible = visible;
        on_changed();
    }

    std::vector<std::weak_ptr<ILight>> Room::lights() const
    {
        return _lights;
    }

    std::vector<std::weak_ptr<ICameraSink>> Room::camera_sinks() const
    {
        return _camera_sinks;
    }

    std::vector<std::weak_ptr<ITrigger>> Room::triggers() const 
    {
        std::vector<std::weak_ptr<ITrigger>> triggers;
        for (const auto& t : _triggers)
        {
            triggers.push_back(t.second);
        }
        return triggers;
    }

    std::vector<std::weak_ptr<IItem>> Room::items() const
    {
        return _entities;
    }

    std::weak_ptr<ILevel> Room::level() const
    {
        return _level;
    }

    Colour Room::ambient() const
    {
        return _ambient;
    }

    int16_t Room::ambient_intensity_1() const
    {
        return _ambient_intensity_1;
    }

    int16_t Room::ambient_intensity_2() const
    {
        return _ambient_intensity_2;
    }

    int16_t Room::light_mode() const
    {
        return _light_mode;
    }

    std::vector<std::weak_ptr<IStaticMesh>> Room::static_meshes() const
    {
        return { std::from_range, _static_meshes };
    }

    void Room::update(float delta)
    {
        if (_mesh)
        {
            _mesh->update(delta);
        }
    }

    uint16_t Room::water_scheme() const
    {
        return _water_scheme;
    }

    int32_t Room::filterable_index() const
    {
        return static_cast<int32_t>(number());
    }

    std::shared_ptr<ISector> sector_from_point(const IRoom& room, const Vector3& point)
    {
        const auto info = room.info();
        const auto extents = Vector3(room.num_x_sectors(), (info.yBottom - info.yTop) / trlevel::Scale, room.num_z_sectors()) * 0.5f;
        const auto box = DirectX::BoundingBox(room.centre(), extents);
        if (!box.Contains(point))
        {
            return nullptr;
        }

        const auto min_bounds = room.centre() - extents;
        const auto offset = point - min_bounds;
        int32_t x = static_cast<int32_t>(offset.x);
        int32_t z = static_cast<int32_t>(offset.z);
        const std::size_t id = static_cast<std::size_t>(x) * room.num_z_sectors() + z;
        const auto sectors = room.sectors();
        if (id < sectors.size())
        {
            return sectors[id];
        }
        return {};
    }

    std::string to_string(IRoom::AlternateMode mode)
    {
        switch (mode)
        {
        case IRoom::AlternateMode::HasAlternate:
            return "has_alternate";
            break;
        case IRoom::AlternateMode::IsAlternate:
            return "is_alternate";
            break;
        default:
            return "none";
            break;
        }
    }

    std::string light_mode_name(int16_t light_mode)
    {
        switch (light_mode)
        {
        case 0:
            return "Default";
        case 1:
            return "Flickering";
        case 2:
            return "Fade";
        case 3:
            return "Sunset";
        }
        return "Unknown";
    }

    uint32_t room_number(const std::weak_ptr<IRoom>& room)
    {
        if (auto room_ptr = room.lock())
        {
            return room_ptr->number();
        }
        return 0u;
    }
}

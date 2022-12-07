#include "Room.h"
#include <trview.app/Geometry/MeshVertex.h>
#include <trview.app/Camera/ICamera.h>
#include <trview.app/Elements/ILevel.h>
#include <trview.common/Algorithms.h>
#include <format>
#include <trview.common/Logs/Activity.h>

using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const Color Trigger_Colour{ 1, 0, 1, 0.5f };
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
    }

    IRoom::~IRoom()
    {
    }

    Room::Room(const IMesh::Source& mesh_source,
        const trlevel::ILevel& level, 
        const trlevel::tr3_room& room,
        std::shared_ptr<ILevelTextureStorage> texture_storage,
        const IMeshStorage& mesh_storage,
        uint32_t index,
        const ILevel& parent_level,
        const Activity& activity,
        const IStaticMesh::MeshSource& static_mesh_mesh_source,
        const IStaticMesh::PositionSource& static_mesh_position_source,
        const ISector::Source& sector_source)
        : _info { room.info.x, 0, room.info.z, room.info.yBottom, room.info.yTop }, 
        _alternate_room(room.alternate_room),
        _alternate_group(room.alternate_group),
        _num_x_sectors(room.num_x_sectors),
        _num_z_sectors(room.num_z_sectors),
        _index(index),
        _flags(room.flags),
        _level(parent_level),
        _texture_storage(texture_storage),
        _mesh_source(mesh_source)
    {
        // Can only determine HasAlternate or normal at this point. After all rooms have been loaded,
        // the level can fix up the rooms so that they know if they are alternates of another room
        // (IsAlternate).
        _alternate_mode = room.alternate_room != -1 ? AlternateMode::HasAlternate : AlternateMode::None;

        _room_offset = Matrix::CreateTranslation(room.info.x / trlevel::Scale_X, 0, room.info.z / trlevel::Scale_Z);
        _inverted_room_offset = _room_offset.Invert();

        generate_sectors(level, room, sector_source);
        generate_geometry(level.get_version(), mesh_source, room);
        generate_adjacency();
        generate_static_meshes(mesh_source, level, room, mesh_storage, static_mesh_mesh_source, static_mesh_position_source, activity);

        _token_store += _level.on_geometry_colours_changed += [&]() { _all_geometry_meshes.clear(); };
    }

    RoomInfo Room::info() const
    {
        return _info;
    }

    std::set<uint16_t> Room::neighbours() const
    {
        return _neighbours;
    }

    PickResult Room::pick(const Vector3& position, const Vector3& direction, PickFilter filters) const
    {
        using namespace DirectX::TriangleTests;

        // Test against bounding box for the room first, to avoid more expensive mesh-ray intersection
        float box_distance = 0;
        if (!_bounding_box.Intersects(position, direction, box_distance))
        {
            return PickResult();
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

                auto entity_result = entity_ptr->pick(position, direction);
                if (entity_result.hit)
                {
                    pick_results.push_back(entity_result);
                }
            }
        }

        if (has_flag(filters, PickFilter::Lights) && pick_results.empty())
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

        if (has_flag(filters, PickFilter::Triggers) && pick_results.empty())
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
                PickResult static_mesh_result = static_mesh->pick(position, direction);
                if (static_mesh_result.hit)
                {
                    static_mesh_result.type = PickResult::Type::Room;
                    static_mesh_result.index = _index;
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

        if (pick_results.empty())
        {
            return PickResult();
        }

        // Choose the closest pick out of all results.
        std::sort(pick_results.begin(), pick_results.end(),
            [](const auto& l, const auto& r) { return l.distance < r.distance; });
        return pick_results.front();
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

                for (const auto& mesh : _all_geometry_meshes)
                {
                    if (mesh.first == _index || (visible_rooms.find(mesh.first) == visible_rooms.end() || _index < mesh.first))
                    {
                        mesh.second->render(_room_offset * camera.view_projection(), *_texture_storage, colour, 1.0f, Vector3::Zero, has_flag(render_filter, RenderFilter::AllGeometry));
                    }
                }
            }
            else
            {
                _mesh->render(_room_offset * camera.view_projection(), *_texture_storage, colour, 1.0f, Vector3::Zero);
                for (const auto& mesh : _static_meshes)
                {
                    mesh->render(camera, *_texture_storage, colour);
                }
            }
        }

        render_contained(camera, colour, render_filter);
    }

    void Room::render_bounding_boxes(const ICamera& camera)
    {
        for (const auto& mesh : _static_meshes)
        {
            mesh->render_bounding_box(camera, *_texture_storage, Colour::White);
        }
    }

    void Room::render_lights(const ICamera& camera, const std::weak_ptr<ILight>& selected_light)
    {
        const auto selected = selected_light.lock();
        for (const auto& light : _lights)
        {
            if (auto light_ptr = light.lock())
            {
                light_ptr->render(camera, *_texture_storage, Colour::White);
                if (light_ptr == selected)
                {
                    light_ptr->render_direction(camera, *_texture_storage);
                }
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

        for (const auto& entity : _entities)
        {
            if (auto entity_ptr = entity.lock())
            {
                entity_ptr->render(camera, *_texture_storage, colour);
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
                activity.log(trview::Message::Status::Error, std::format("Static Mesh {} was requested but not found", room_mesh.mesh_id));
                continue;
            }
            _static_meshes.push_back(static_mesh_mesh_source(room_mesh, level_static_mesh.value(), mesh_storage.mesh(level_static_mesh.value().Mesh)));
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
            _static_meshes.push_back(static_mesh_position_source(pos, scale, sprite_mesh));
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

    void Room::generate_geometry(trlevel::LevelVersion level_version, const IMesh::Source& mesh_source, const trlevel::tr3_room& room)
    {
        std::vector<trlevel::tr_vertex> room_vertices;
        std::transform(room.data.vertices.begin(), room.data.vertices.end(), std::back_inserter(room_vertices),
            [](const auto& v) { return v.vertex; });

        std::vector<MeshVertex> vertices;
        std::vector<TransparentTriangle> transparent_triangles;

        // The indices are grouped by the number of textiles so that it can be drawn as the selected texture.
        std::vector<std::vector<uint32_t>> indices(_texture_storage->num_tiles());
        
        std::vector<Triangle> collision_triangles;

        process_textured_rectangles(level_version, room.data.rectangles, room_vertices, *_texture_storage, vertices, indices, transparent_triangles, collision_triangles, false);
        process_textured_triangles(level_version, room.data.triangles, room_vertices, *_texture_storage, vertices, indices, transparent_triangles, collision_triangles, false);
        process_collision_transparency(transparent_triangles, collision_triangles);

        _mesh = mesh_source(vertices, indices, std::vector<uint32_t>{}, transparent_triangles, collision_triangles);

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

    void Room::add_entity(const std::weak_ptr<IEntity>& entity)
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
        _triggers.insert({ trigger_ptr->sector_id(), trigger });
    }

    void Room::add_light(const std::weak_ptr<ILight>& light)
    {
        _lights.push_back(light);

        // Place suns in the middle of the room instead of 9 million units away.
        // Only do this if the light position is massive - don't change Tomb Editor placed suns.
        // Stack them in a line so they don't overlap each other.
        auto light_ptr = light.lock();
        if (light_ptr && light_ptr->type() == trlevel::LightType::Sun && light_ptr->position().x > 9000)
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

    void Room::generate_sectors(const trlevel::ILevel& level, const trlevel::tr3_room& room, const ISector::Source& sector_source)
    {
        for (auto i = 0u; i < room.sector_list.size(); ++i)
        {
            const trlevel::tr_room_sector &sector = room.sector_list[i];
            _sectors.push_back(sector_source(level, room, sector, i, _index, *this));
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
                    transparency.add(triangle.transform(_room_offset, colour));
                }

                for (const auto& static_mesh : _static_meshes)
                {
                    static_mesh->get_transparent_triangles(transparency, camera, colour);
                }
            }
        }

        if (has_flag(render_filter, RenderFilter::Triggers))
        {
            for (const auto& trigger_pair : _triggers)
            {
                if (auto trigger = trigger_pair.second.lock())
                {
                    trigger->get_transparent_triangles(transparency, camera, Trigger_Colour);
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
                entity_ptr->get_transparent_triangles(transparency, camera, colour);
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

            if (auto other = get_trigger_sector(trigger->x() + 1, trigger->z()))
            {
                auto corners = other->corners();
                if (y_bottom[3] == corners[1] && y_bottom[2] == corners[0])
                {
                    pos_x = false;
                }
            }

            if (auto other = get_trigger_sector(static_cast<int32_t>(trigger->x()) - 1, trigger->z()))
            {
                auto corners = other->corners();
                if (y_bottom[1] == corners[3] && y_bottom[0] == corners[2])
                {
                    neg_x = false;
                }
            }

            if (auto other = get_trigger_sector(trigger->x(), static_cast<int32_t>(trigger->z()) - 1))
            {
                auto corners = other->corners();
                if (y_bottom[2] == corners[3] && y_bottom[0] == corners[1])
                {
                    neg_z = false;
                }
            }

           if (auto other = get_trigger_sector(trigger->x(), trigger->z() + 1))
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

            std::vector<TransparentTriangle> triangles;

            // + Y
            triangles.push_back(TransparentTriangle(Vector3(x + 0.5f, y_top[3], z + 0.5f), Vector3(x + 0.5f, y_top[2], z - 0.5f), Vector3(x - 0.5f, y_top[0], z - 0.5f), Trigger_Colour));
            triangles.push_back(TransparentTriangle(Vector3(x - 0.5f, y_top[0], z - 0.5f), Vector3(x - 0.5f, y_top[1], z + 0.5f), Vector3(x + 0.5f, y_top[3], z + 0.5f), Trigger_Colour));

            if (pos_x)
            {
                // + X
                triangles.push_back(TransparentTriangle(Vector3(x + 0.5f, y_top[2], z - 0.5f), Vector3(x + 0.5f, y_top[3], z + 0.5f), Vector3(x + 0.5f, y_bottom[3], z + 0.5f), Trigger_Colour));
                triangles.push_back(TransparentTriangle(Vector3(x + 0.5f, y_top[2], z - 0.5f), Vector3(x + 0.5f, y_bottom[3], z + 0.5f), Vector3(x + 0.5f, y_bottom[2], z - 0.5f), Trigger_Colour));
            }

            if (neg_x)
            {
                // - X
                triangles.push_back(TransparentTriangle(Vector3(x - 0.5f, y_top[1], z + 0.5f), Vector3(x - 0.5f, y_top[0], z - 0.5f), Vector3(x - 0.5f, y_bottom[0], z - 0.5f), Trigger_Colour));
                triangles.push_back(TransparentTriangle(Vector3(x - 0.5f, y_top[1], z + 0.5f), Vector3(x - 0.5f, y_bottom[0], z - 0.5f), Vector3(x - 0.5f, y_bottom[1], z + 0.5f), Trigger_Colour));
            }

            if (pos_z)
            {
                // + Z
                triangles.push_back(TransparentTriangle(Vector3(x + 0.5f, y_top[3], z + 0.5f), Vector3(x - 0.5f, y_top[1], z + 0.5f), Vector3(x - 0.5f, y_bottom[1], z + 0.5f), Trigger_Colour));
                triangles.push_back(TransparentTriangle(Vector3(x + 0.5f, y_top[3], z + 0.5f), Vector3(x - 0.5f, y_bottom[1], z + 0.5f), Vector3(x + 0.5f, y_bottom[3], z + 0.5f), Trigger_Colour));
            }

            if (neg_z)
            {
                // - Z
                triangles.push_back(TransparentTriangle(Vector3(x - 0.5f, y_top[0], z - 0.5f), Vector3(x + 0.5f, y_top[2], z - 0.5f), Vector3(x + 0.5f, y_bottom[2], z - 0.5f), Trigger_Colour));
                triangles.push_back(TransparentTriangle(Vector3(x - 0.5f, y_top[0], z - 0.5f), Vector3(x + 0.5f, y_bottom[2], z - 0.5f), Vector3(x - 0.5f, y_bottom[0], z - 0.5f), Trigger_Colour));
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

    ISector* Room::get_trigger_sector(int32_t x, int32_t z)
    {
        auto sector_id = get_sector_id(x, z);
        auto trigger = _triggers.find(sector_id);
        if (trigger != _triggers.end())
        {
            return _sectors[sector_id].get();
        }

        if (sector_id >= _sectors.size())
        {
            return nullptr;
        }

        // Check if this sector is a portal.
        auto sector = _sectors[sector_id];
        if (!has_flag(sector->flags(), SectorFlag::Portal))
        {
            return nullptr;
        }

        auto room = _level.room(sector->portal()).lock();
        if (!room)
        {
            return nullptr;
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

        return nullptr;
    }

    namespace
    {
        void add_triangle(
            const ISector::Triangle& tri,
            std::vector<MeshVertex>& output_vertices,
            std::vector<uint32_t>& output_indices,
            std::vector<Triangle>& collision_triangles,
            const Color& color)
        {
            uint32_t base = static_cast<uint32_t>(output_vertices.size());

            output_vertices.push_back({ tri.v0, Vector3::Down, tri.uv0, color });
            output_vertices.push_back({ tri.v1, Vector3::Down, tri.uv1, color });
            output_vertices.push_back({ tri.v2, Vector3::Down, tri.uv2, color });

            output_indices.push_back(base);
            output_indices.push_back(base + 1);
            output_indices.push_back(base + 2);

            collision_triangles.emplace_back(tri.v0, tri.v1, tri.v2);
        }
    }

    void Room::process_collision_transparency(const std::vector<TransparentTriangle>& transparent_triangles, std::vector<Triangle>& collision_triangles)
    {
        for (const auto& triangle : transparent_triangles)
        {
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
                    collision_triangles.push_back(Triangle(triangle.vertices[0], triangle.vertices[1], triangle.vertices[2]));

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
        return _level.version() == trlevel::LevelVersion::Tomb3 && (_flags & 0x80);
    }

    const std::vector<std::shared_ptr<ISector>> Room::sectors() const
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
        geometry_result.index = _index;
        geometry_result.position = Vector3::Transform(geometry_result.position, _room_offset);

        const auto& tri = geometry_result.triangle;
        if (tri.normal.y < 0)
        {
            Vector3 centroid = { std::floor(geometry_result.position.x) + 0.5f, geometry_result.position.y, std::floor(geometry_result.position.z) + 0.5f };
            Vector3 ray_direction = { 0, -tri.normal.y, 0 };

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
        const auto colours = _level.map_colours();

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
            std::vector<MeshVertex> vertices;
            std::vector<Triangle> collision_triangles;
            std::vector<uint32_t> untextured_indices;
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
                add_triangle(tri, part.vertices, part.untextured_indices, part.collision_triangles, colour);
            }
            base += tris.size();
        }

        for (const auto& parts : mesh_parts)
        {
            std::vector<std::vector<uint32_t>> vec;
            vec.push_back(parts.second.untextured_indices);
            auto mesh = mesh_source(parts.second.vertices, vec, std::vector<uint32_t>{}, std::vector<TransparentTriangle>{}, parts.second.collision_triangles);
            _all_geometry_meshes[parts.first] = mesh;
        }
    }

    ISector::Portal Room::sector_portal(int x1, int z1, int x2, int z2) const
    {
        ISector::Portal portal;
        auto sector = _sectors[get_sector_id(x1, z1)];
        if (sector->room_above() != 0xff)
        {
            const auto other_room = _level.room(sector->room_above()).lock();
            const auto diff = (position() - other_room->position()) + Vector3(static_cast<float>(x1), 0, static_cast<float>(z1));
            const int other_id = static_cast<int>(diff.x * other_room->num_z_sectors() + diff.z);
            portal.sector_above = other_room->sectors()[other_id];
            portal.above_offset = Vector3(static_cast<float>(x1), 0, static_cast<float>(z1)) - diff;
            portal.room_above = other_room;
        }

        const auto id = get_sector_id(x2, z2);
        if (x2 >= _num_x_sectors || x2 < 0 || z2 >= _num_z_sectors || z2 < 0)
        {
            return portal;
        }

        portal.direct = _sectors[id];
        portal.direct_room = std::const_pointer_cast<IRoom>(shared_from_this());
        if (has_flag(portal.direct->flags(), SectorFlag::Portal))
        {
            const auto other_room = _level.room(portal.direct->portal()).lock();
            const auto diff = (position() - other_room->position()) + Vector3(static_cast<float>(x2), 0, static_cast<float>(z2));
            const int other_id = static_cast<int>(diff.x * other_room->num_z_sectors() + diff.z);
            const auto sectors = other_room->sectors();
            if (other_id < sectors.size())
            {
                portal.target = sectors[other_id];
                portal.offset = Vector3(static_cast<float>(x2), 0, static_cast<float>(z2)) - diff;
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
    }
}

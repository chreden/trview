#include "Room.h"
#include <trview.app/Geometry/MeshVertex.h>
#include <trview.app/Camera/ICamera.h>
#include <trview.app/Elements/ILevel.h>

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

    Room::Room(const IMesh::Source& mesh_source,
        const trlevel::ILevel& level, 
        const trlevel::tr3_room& room,
        std::shared_ptr<ILevelTextureStorage> texture_storage,
        const IMeshStorage& mesh_storage,
        uint32_t index,
        const ILevel& parent_level,
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
        _texture_storage(texture_storage)
    {
        // Can only determine HasAlternate or normal at this point. After all rooms have been loaded,
        // the level can fix up the rooms so that they know if they are alternates of another room
        // (IsAlternate).
        _alternate_mode = room.alternate_room != -1 ? AlternateMode::HasAlternate : AlternateMode::None;

        _room_offset = Matrix::CreateTranslation(room.info.x / trlevel::Scale_X, 0, room.info.z / trlevel::Scale_Z);
        generate_sectors(level, room, sector_source);
        generate_geometry(level.get_version(), mesh_source, room);
        generate_adjacency();
        generate_static_meshes(mesh_source, level, room, mesh_storage, static_mesh_mesh_source, static_mesh_position_source);
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

        if (has_flag(filters, PickFilter::StaticMeshes))
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

        if (has_flag(filters, PickFilter::Geometry))
        {
            // Pick against the room geometry:
            auto room_offset = Matrix::CreateTranslation(-_info.x / trlevel::Scale_X, 0, -_info.z / trlevel::Scale_Z);
            PickResult geometry_result = _mesh->pick(Vector3::Transform(position, room_offset), direction);
            if (geometry_result.hit)
            {
                // Transform the position back in to world space. Also mark it as a room pick result.
                geometry_result.type = PickResult::Type::Room;
                geometry_result.index = _index;
                geometry_result.position = Vector3::Transform(geometry_result.position, _room_offset);
                pick_results.push_back(geometry_result);
            }

            if (has_flag(filters, PickFilter::HiddenGeometry))
            {
                PickResult unmatched_result = _unmatched_mesh->pick(Vector3::Transform(position, room_offset), direction);
                if (unmatched_result.hit)
                {
                    unmatched_result.type = PickResult::Type::Room;
                    unmatched_result.index = _index;
                    unmatched_result.position = Vector3::Transform(unmatched_result.position, _room_offset);
                    pick_results.push_back(unmatched_result);
                }
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

    // Render the level geometry and the objects contained in this room.
    // context: The D3D context.
    // camera: The camera to use to render.
    // texture_storage: The textures for the level.
    // selected: The selection mode to use to highlight geometry and objects.
    // render_mode: The type of geometry and object geometry to render.
    void Room::render(const ICamera& camera, SelectionMode selected, bool show_hidden_geometry, bool show_water)
    {
        Color colour = room_colour(water() && show_water, selected);

        _mesh->render(_room_offset * camera.view_projection(), *_texture_storage, colour);
        if (show_hidden_geometry)
        {
            _unmatched_mesh->render(_room_offset * camera.view_projection(), *_texture_storage, colour);
        }

        for (const auto& mesh : _static_meshes)
        {
            mesh->render(camera, *_texture_storage, colour);
        }

        render_contained(camera, colour);
    }

    void Room::render_contained(const ICamera& camera, SelectionMode selected, bool show_water)
    {
        Color colour = room_colour(water() && show_water, selected);
        render_contained(camera, colour);
    }

    void Room::render_contained(const ICamera& camera, const Color& colour)
    {
        for (const auto& entity : _entities)
        {
            if (auto entity_ptr = entity.lock())
            {
                entity_ptr->render(camera, *_texture_storage, colour);
            }
        }
    }

    void Room::generate_static_meshes(const IMesh::Source& mesh_source, const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage,
        const IStaticMesh::MeshSource& static_mesh_mesh_source, const IStaticMesh::PositionSource& static_mesh_position_source)
    {
        for (uint32_t i = 0; i < room.static_meshes.size(); ++i)
        {
            auto room_mesh = room.static_meshes[i];
            auto level_static_mesh = level.get_static_mesh(room_mesh.mesh_id);
            _static_meshes.push_back(static_mesh_mesh_source(room_mesh, level_static_mesh, mesh_storage.mesh(level_static_mesh.Mesh)));
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

        // Make the unmatched mesh.
        collision_triangles.clear();
        vertices.clear();
        std::vector<uint32_t> untextured_indices;
        process_unmatched_geometry(room.data, room_vertices, transparent_triangles, vertices, untextured_indices, collision_triangles);
        _unmatched_mesh = mesh_source(vertices, std::vector<std::vector<uint32_t>>{}, untextured_indices, std::vector<TransparentTriangle>{}, collision_triangles);

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

    void 
    Room::generate_sectors(const trlevel::ILevel& level, const trlevel::tr3_room& room, const ISector::Source& sector_source)
    {
        for (auto i = 0u; i < room.sector_list.size(); ++i)
        {
            const trlevel::tr_room_sector &sector = room.sector_list[i];
            _sectors.push_back(sector_source(level, room, sector, i, _index));
        }
    }

    void Room::get_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool include_triggers, bool show_water)
    {
        Color colour = room_colour(water() && show_water, selected);

        for (const auto& triangle : _mesh->transparent_triangles())
        {
            transparency.add(triangle.transform(_room_offset, colour));
        }

        for (const auto& static_mesh : _static_meshes)
        {
            static_mesh->get_transparent_triangles(transparency, camera, colour);
        }

        if (include_triggers)
        {
            for (const auto& trigger_pair : _triggers)
            {
                if (auto trigger = trigger_pair.second.lock())
                {
                    trigger->get_transparent_triangles(transparency, camera, Trigger_Colour);
                }
            }
        }

        get_contained_transparent_triangles(transparency, camera, colour);
    }

    void Room::get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool show_water)
    {
        Color colour = room_colour(water() && show_water, selected);
        get_contained_transparent_triangles(transparency, camera, colour);
    }

    void Room::get_contained_transparent_triangles(ITransparencyBuffer& transparency, const ICamera& camera, const Color& colour)
    {
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
        if (!(sector->flags() & SectorFlag::Portal))
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
        /// Check if the triangle appears in any of the rectangles or triangles in the room.
        /// @param triangle The points in the sector triangle.
        /// @param data The room data containing the rectangles and triangles.
        /// @param room_vertices The vertices for the room.
        /// @param transparent_triangles Transparent triangles in the room.
        bool geometry_matched(const std::vector<Vector3>& triangle, const trlevel::tr3_room_data& data,  const std::vector<Vector3>& room_vertices, const std::vector<TransparentTriangle>& transparent_triangles)
        {
            for (const auto& r : data.rectangles)
            {
                if (triangle_contained(triangle,
                        {
                            room_vertices[r.vertices[0]],
                            room_vertices[r.vertices[1]],
                            room_vertices[r.vertices[2]],
                            room_vertices[r.vertices[3]]
                        }))
                {
                    return true;
                }
            }

            for (const auto& t : data.triangles)
            {
                if (triangle_contained(triangle,
                        {
                            room_vertices[t.vertices[0]],
                            room_vertices[t.vertices[1]],
                            room_vertices[t.vertices[2]]
                        }))
                {
                    return true;
                }
            }

            for (const auto& tt : transparent_triangles)
            {
                if (triangle_contained(triangle, { tt.vertices, tt.vertices + 3 }))
                {
                    return true;
                }
            }

            return false;
        }

        void add_triangle(
            const std::vector<Vector3>& tri,
            std::vector<MeshVertex>& output_vertices,
            std::vector<uint32_t>& output_indices,
            std::vector<Triangle>& collision_triangles,
            const Color& color)
        {
            uint32_t base = static_cast<uint32_t>(output_vertices.size());

            output_vertices.push_back({ tri[0], Vector3::Down, Vector2::Zero, color });
            output_vertices.push_back({ tri[1], Vector3::Down, Vector2::Zero, color });
            output_vertices.push_back({ tri[2], Vector3::Down, Vector2::Zero, color });

            output_indices.push_back(base);
            output_indices.push_back(base + 1);
            output_indices.push_back(base + 2);

            collision_triangles.emplace_back(tri[0], tri[1], tri[2]);
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

    void Room::process_unmatched_geometry(
        const trlevel::tr3_room_data& data,
        const std::vector<trlevel::tr_vertex>& room_vertices,
        const std::vector<TransparentTriangle>& transparent_triangles,
        std::vector<MeshVertex>& output_vertices,
        std::vector<uint32_t>& output_indices,
        std::vector<Triangle>& collision_triangles)
    {
        std::vector<Vector3> transformed_room_vertices;
        std::transform(room_vertices.begin(), room_vertices.end(), std::back_inserter(transformed_room_vertices), convert_vertex);

        for (const auto& sector : _sectors)
        {
            if (sector->is_floor())
            {
                const auto tris = sector->triangles();
                if (!geometry_matched({ tris.begin(), tris.begin() + 3 }, data, transformed_room_vertices, transparent_triangles))
                {
                    add_triangle({ tris.begin(), tris.begin() + 3 }, output_vertices, output_indices, collision_triangles, get_unmatched_colour(_info, *sector));
                }

                if (!geometry_matched({ tris.begin() + 3, tris.end() }, data, transformed_room_vertices, transparent_triangles))
                {
                    add_triangle({ tris.begin() + 3, tris.end() }, output_vertices, output_indices, collision_triangles, get_unmatched_colour(_info, *sector));
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
}

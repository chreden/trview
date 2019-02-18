#include "stdafx.h"
#include "Room.h"
#include <trview.app/MeshVertex.h>
#include "Entity.h"

#include <trview.app/ILevelTextureStorage.h>
#include "IMeshStorage.h"
#include <trview.app/ICamera.h>
#include <trview.app/Mesh.h>
#include <trview.app/TransparencyBuffer.h>

#include <SimpleMath.h>
#include <DirectXCollision.h>
#include <array>
#include <iterator>
#include <numeric>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        const Color Trigger_Colour{ 1, 0, 1, 0.5f };
    }

    Room::Room(const graphics::Device& device, 
        const trlevel::ILevel& level, 
        const trlevel::tr3_room& room,
        const ILevelTextureStorage& texture_storage,
        const IMeshStorage& mesh_storage,
        uint32_t index)
        : _info { room.info.x, 0, room.info.z, room.info.yBottom, room.info.yTop }, 
        _alternate_room(room.alternate_room),
        _alternate_group(room.alternate_group),
        _num_x_sectors(room.num_x_sectors),
        _num_z_sectors(room.num_z_sectors),
        _index(index)
    {
        // Can only determine HasAlternate or normal at this point. After all rooms have been loaded,
        // the level can fix up the rooms so that they know if they are alternates of another room
        // (IsAlternate).
        _alternate_mode = room.alternate_room != -1 ? AlternateMode::HasAlternate : AlternateMode::None;

        _room_offset = Matrix::CreateTranslation(room.info.x / trlevel::Scale_X, 0, room.info.z / trlevel::Scale_Z);
        generate_geometry(level.get_version(), device, room, texture_storage);
        generate_sectors(level, room);
        generate_adjacency();
        generate_static_meshes(level, room, mesh_storage);
    }

    RoomInfo Room::info() const
    {
        return _info;
    }

    std::set<uint16_t> Room::neighbours() const
    {
        return _neighbours;
    }

    // Determine whether the specified ray hits any of the triangles in the room geometry.
    // position: The world space position of the source of the ray.
    // direction: The direction of the ray.
    // Returns: The result of the operation. If 'hit' is true, distance and position contain
    // how far along the ray the hit was and the position in world space.
    PickResult Room::pick(const Vector3& position, const Vector3& direction, bool include_entities, bool include_triggers) const
    {
        using namespace DirectX::TriangleTests;

        // Test against bounding box for the room first, to avoid more expensive mesh-ray intersection
        float box_distance = 0;
        if (!_bounding_box.Intersects(position, direction, box_distance))
        {
            return PickResult();
        }

        std::vector<PickResult> pick_results;

        if (include_entities)
        {
            // Pick against the entity geometry:
            for (const auto& entity : _entities)
            {
                auto entity_result = entity->pick(position, direction);
                if (entity_result.hit)
                {
                    pick_results.push_back(entity_result);
                }
            }
        }

        if (include_triggers && pick_results.empty())
        {
            for (const auto& trigger : _triggers)
            {
                auto trigger_result = trigger.second->pick(position, direction);
                if (trigger_result.hit)
                {
                    pick_results.push_back(trigger_result);
                }
            }
        }

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
    void Room::render(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected)
    {
        Color colour = selected == SelectionMode::Selected ? Color(1, 1, 1, 1) :
            selected == SelectionMode::Neighbour ? Color(0.4f, 0.4f, 0.4f, 1) : Color(0.2f, 0.2f, 0.2f, 1);

        auto context = device.context();

        _mesh->render(context, _room_offset * camera.view_projection(), texture_storage, colour);

        for (const auto& mesh : _static_meshes)
        {
            mesh->render(context, camera.view_projection(), texture_storage, colour);
        }

        render_contained(device, camera, texture_storage, colour);
    }

    void Room::render_contained(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected)
    {
        Color colour = selected == SelectionMode::Selected ? Color(1, 1, 1, 1) :
            selected == SelectionMode::Neighbour ? Color(0.4f, 0.4f, 0.4f, 1) : Color(0.2f, 0.2f, 0.2f, 1);
        render_contained(device, camera, texture_storage, colour);
    }

    void Room::render_contained(const graphics::Device& device, const ICamera& camera, const ILevelTextureStorage& texture_storage, const Color& colour)
    {
        for (const auto& entity : _entities)
        {
            entity->render(device, camera, texture_storage, colour);
        }
    }

    void Room::generate_static_meshes(const trlevel::ILevel& level, const trlevel::tr3_room& room, const IMeshStorage& mesh_storage)
    {
        for (uint32_t i = 0; i < room.static_meshes.size(); ++i)
        {
            auto room_mesh = room.static_meshes[i];
            auto level_static_mesh = level.get_static_mesh(room_mesh.mesh_id);
            auto static_mesh = std::make_unique<StaticMesh>(room_mesh, level_static_mesh, mesh_storage.mesh(level_static_mesh.Mesh));
            _static_meshes.push_back(std::move(static_mesh));
        }
    }

    void Room::generate_geometry(trlevel::LevelVersion level_version, const graphics::Device& device, const trlevel::tr3_room& room, const ILevelTextureStorage& texture_storage)
    {
        std::vector<trlevel::tr_vertex> room_vertices;
        std::transform(room.data.vertices.begin(), room.data.vertices.end(), std::back_inserter(room_vertices),
            [](const auto& v) { return v.vertex; });

        std::vector<MeshVertex> vertices;
        std::vector<TransparentTriangle> transparent_triangles;

        // The indices are grouped by the number of textiles so that it can be drawn as the selected texture.
        std::vector<std::vector<uint32_t>> indices(texture_storage.num_tiles());
        std::vector<Triangle> collision_triangles;

        process_textured_rectangles(level_version, room.data.rectangles, room_vertices, texture_storage, vertices, indices, transparent_triangles, collision_triangles, false);
        process_textured_triangles(level_version, room.data.triangles, room_vertices, texture_storage, vertices, indices, transparent_triangles, collision_triangles, false);

        _mesh = std::make_unique<Mesh>(device, vertices, indices, std::vector<uint32_t>(), transparent_triangles, collision_triangles);
        
        // Generate the bounding box based on the room dimensions.
        const auto extents = Vector3(_num_x_sectors, (_info.yBottom - _info.yTop) / trlevel::Scale_Y, _num_z_sectors) * 0.5f;
        _bounding_box = DirectX::BoundingBox(centre(), extents);
    }

    void Room::generate_adjacency()
    {
        _neighbours.clear(); 

        std::for_each(_sectors.begin(), _sectors.end(), [&] (const auto& pair) {
            const std::set<std::uint16_t> n = pair.second->neighbours(); 
            _neighbours.insert(n.begin(), n.end());
        });
    }

    void Room::add_entity(Entity* entity)
    {
        _entities.push_back(entity);
    }

    void Room::add_trigger(Trigger* trigger)
    {
        _triggers.insert({ trigger->sector_id(), trigger });
    }

    void 
    Room::generate_sectors(const trlevel::ILevel& level, const trlevel::tr3_room& room)
    {
        for (int i = 0; i < room.sector_list.size(); ++i)
        {
            const trlevel::tr_room_sector &sector = room.sector_list.at(i);
            _sectors.emplace(std::make_pair(
                i, std::make_shared<Sector>(level, room, sector, i, _index)
            ));
        }
    }

    void Room::get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected, bool include_triggers)
    {
        Color colour = selected == SelectionMode::Selected ? Color(1, 1, 1, 1) :
            selected == SelectionMode::Neighbour ? Color(0.4f, 0.4f, 0.4f, 1) : Color(0.2f, 0.2f, 0.2f, 1);

        for (const auto& triangle : _mesh->transparent_triangles())
        {
            transparency.add(triangle.transform(_room_offset, colour));
        }

        for (const auto& static_mesh : _static_meshes)
        {
            static_mesh->get_transparent_triangles(transparency, colour);
        }

        if (include_triggers)
        {
            for (const auto& trigger : _triggers)
            {
                for (const auto& triangle : trigger.second->triangles())
                {
                    transparency.add(triangle);
                }
            }
        }

        get_contained_transparent_triangles(transparency, camera, colour);
    }

    void Room::get_contained_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected)
    {
        Color colour = selected == SelectionMode::Selected ? Color(1, 1, 1, 1) :
            selected == SelectionMode::Neighbour ? Color(0.4f, 0.4f, 0.4f, 1) : Color(0.2f, 0.2f, 0.2f, 1);
        get_contained_transparent_triangles(transparency, camera, colour);
    }

    void Room::get_contained_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, const Color& colour)
    {
        for (const auto& entity : _entities)
        {
            entity->get_transparent_triangles(transparency, camera, colour);
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

    const DirectX::BoundingBox& Room::bounding_box() const
    {
        return _bounding_box;
    }

    void Room::generate_trigger_geometry()
    {
        for (auto& trigger_iter : _triggers)
        {
            // Information about sector height.
            auto trigger = trigger_iter.second;
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

    Sector* Room::get_trigger_sector(int32_t x, int32_t z)
    {
        auto sector_id = get_sector_id(x, z);
        auto trigger = _triggers.find(sector_id);
        if (trigger == _triggers.end())
        {
            return nullptr;
        }
        return _sectors[sector_id].get();
    }
}

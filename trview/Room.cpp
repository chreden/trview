#include "stdafx.h"
#include "Room.h"
#include "MeshVertex.h"
#include "Entity.h"

#include "ILevelTextureStorage.h"
#include "IMeshStorage.h"
#include "ICamera.h"
#include "Mesh.h"
#include "TransparencyBuffer.h"

#include <SimpleMath.h>
#include <DirectXCollision.h>
#include <array>
#include <iterator>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    Room::Room(const ComPtr<ID3D11Device>& device, 
        const trlevel::ILevel& level, 
        const trlevel::tr3_room& room,
        const ILevelTextureStorage& texture_storage,
        const IMeshStorage& mesh_storage,
        uint32_t index)
        : _info { room.info.x, 0, room.info.z, room.info.yBottom, room.info.yTop }, 
        _alternate_room(room.alternate_room),
        _num_x_sectors(room.num_x_sectors),
        _num_z_sectors(room.num_z_sectors),
        _index(index)
    {
        // Can only determine HasAlternate or normal at this point. After all rooms have been loaded,
        // the level can fix up the rooms so that they know if they are alternates of another room
        // (IsAlternate).
        _alternate_mode = room.alternate_room != -1 ? AlternateMode::HasAlternate : AlternateMode::None;

        _room_offset = Matrix::CreateTranslation(room.info.x / 1024.f, 0, room.info.z / 1024.f);
        generate_geometry(device, room, texture_storage);
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
    PickResult Room::pick(const Vector3& position, const Vector3& direction) const
    {
        using namespace DirectX::TriangleTests;

        // Test against bounding box for the room first, to avoid more expensive mesh-ray intersection
        float box_distance = 0;
        if (!_bounding_box.Intersects(position, direction, box_distance))
        {
            return PickResult();
        }

        std::vector<PickResult> pick_results;

        // Pick against the entity geometry:
        for (const auto& entity : _entities)
        {
            auto entity_result = entity->pick(position, direction);
            if (entity_result.hit)
            {
                pick_results.push_back(entity_result);
            }
        }

        // Pick against the room geometry:
        auto room_offset = Matrix::CreateTranslation(-_info.x / 1024.f, 0, -_info.z / 1024.f);
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
    void Room::render(const ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected)
    {
        Color colour = selected == SelectionMode::Selected ? Color(1, 1, 1, 1) :
            selected == SelectionMode::Neighbour ? Color(0.4f, 0.4f, 0.4f, 1) : Color(0.2f, 0.2f, 0.2f, 1);

        _mesh->render(context, _room_offset * camera.view_projection(), texture_storage, colour);

        for (const auto& mesh : _static_meshes)
        {
            mesh->render(context, camera.view_projection(), texture_storage, colour);
        }

        render_contained(context, camera, texture_storage, colour);
    }

    void Room::render_contained(const ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected)
    {
        Color colour = selected == SelectionMode::Selected ? Color(1, 1, 1, 1) :
            selected == SelectionMode::Neighbour ? Color(0.4f, 0.4f, 0.4f, 1) : Color(0.2f, 0.2f, 0.2f, 1);
        render_contained(context, camera, texture_storage, colour);
    }

    void Room::render_contained(const ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, const Color& colour)
    {
        for (const auto& entity : _entities)
        {
            entity->render(context, camera, texture_storage, colour);
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

    void Room::generate_geometry(const ComPtr<ID3D11Device>& device, const trlevel::tr3_room& room, const ILevelTextureStorage& texture_storage)
    {
        std::vector<trlevel::tr_vertex> room_vertices;
        std::transform(room.data.vertices.begin(), room.data.vertices.end(), std::back_inserter(room_vertices),
            [](const auto& v) { return v.vertex; });

        std::vector<MeshVertex> vertices;
        std::vector<TransparentTriangle> transparent_triangles;

        // The indices are grouped by the number of textiles so that it can be drawn as the selected texture.
        std::vector<std::vector<uint32_t>> indices(texture_storage.num_tiles());
        std::vector<Triangle> collision_triangles;

        process_textured_rectangles(room.data.rectangles, room_vertices, texture_storage, vertices, indices, transparent_triangles, collision_triangles, false);
        process_textured_triangles(room.data.triangles, room_vertices, texture_storage, vertices, indices, transparent_triangles, collision_triangles, false);

        _mesh = std::make_unique<Mesh>(device, vertices, indices, std::vector<uint32_t>(), transparent_triangles, collision_triangles);
        _bounding_box = _mesh->bounding_box();
        _bounding_box.Center = centre();
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

    void 
    Room::generate_sectors(const trlevel::ILevel& level, const trlevel::tr3_room& room)
    {
        for (int i = 0; i < room.sector_list.size(); ++i)
        {
            const trlevel::tr_room_sector &sector = room.sector_list.at(i);
            _sectors.emplace(std::make_pair(
                i, std::make_shared<Sector>(level, room, sector, i)
            ));
        }
    }

    void Room::get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected)
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

        // Add trigger cubes.
        TransparentTriangle triangle(Vector3(2, 1, 2), Vector3(2, 1, 1), Vector3(1, 1, 1), Vector2::Zero, Vector2::Zero, Vector2::Zero, 0, TransparentTriangle::Mode::Normal);
        transparency.add(triangle.transform(_room_offset, Color(1, 0, 1, 0.5f)));

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
        return Vector3(_info.x / 1024.f + _num_x_sectors / 2.f,
                 _info.yBottom / -1024.f + (_info.yTop - _info.yBottom) / -1024.f / 2.0f,
                 (_info.z / 1024.f) + _num_z_sectors / 2.f);
    }

    const DirectX::BoundingBox& Room::bounding_box() const
    {
        return _bounding_box;
    }
}

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

namespace trview
{
    Room::Room(CComPtr<ID3D11Device> device, 
        const trlevel::ILevel& level, 
        const trlevel::tr3_room& room,
        const ILevelTextureStorage& texture_storage,
        const IMeshStorage& mesh_storage)
        : _device(device), _info { room.info.x, 0, room.info.z, room.info.yBottom, room.info.yTop }
    {
        _room_offset = DirectX::SimpleMath::Matrix::CreateTranslation(room.info.x / 1024.f, 0, room.info.z / 1024.f);

        generate_geometry(level, room, texture_storage);
        generate_adjacency(level, room);
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
    Room::PickResult Room::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        using namespace DirectX::TriangleTests;
        using namespace DirectX::SimpleMath;

        PickResult result;

        auto room_offset = Matrix::CreateTranslation(-_info.x / 1024.f, 0, -_info.z / 1024.f);
        auto transformed_position = Vector3::Transform(position, room_offset);

        // Test against bounding box for the room first, to avoid more expensive mesh-ray intersection
        float box_distance = 0;
        if (!_bounding_box.Intersects(transformed_position, direction, box_distance))
        {
            return result;
        }

        bool any_hit = false;
        result.distance = FLT_MAX;
        for (const auto& tri : _collision_triangles)
        {
            float distance = 0;
            if (direction.Dot(tri.normal) < 0 &&
                Intersects(transformed_position, direction, tri.v0, tri.v1, tri.v2, distance))
            {
                result.hit = true;
                result.distance = std::min(distance, result.distance);
            }
        }

        // Calculate the world space hit position, if there was a hit.
        if (result.hit)
        {
            result.position = position + direction * result.distance;
        }
        return result;
    }

    // Render the level geometry and the objects contained in this room.
    // context: The D3D context.
    // camera: The camera to use to render.
    // texture_storage: The textures for the level.
    // selected: The selection mode to use to highlight geometry and objects.
    // render_mode: The type of geometry and object geometry to render.
    void Room::render(CComPtr<ID3D11DeviceContext> context, const ICamera& camera, const ILevelTextureStorage& texture_storage, SelectionMode selected)
    {
        using namespace DirectX::SimpleMath;

        Color colour = selected == SelectionMode::Selected ? Color(1, 1, 1, 1) :
            selected == SelectionMode::Neighbour ? Color(0.4f, 0.4f, 0.4f, 1) : Color(0.2f, 0.2f, 0.2f, 1);

        _mesh->render(context, _room_offset * camera.view_projection(), texture_storage, colour);

        for (const auto& mesh : _static_meshes)
        {
            mesh->render(context, camera.view_projection(), texture_storage, colour);
        }

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

    void Room::generate_geometry(const trlevel::ILevel& level, const trlevel::tr3_room& room, const ILevelTextureStorage& texture_storage)
    {
        using namespace DirectX::SimpleMath;

        // Geometry.
        std::vector<MeshVertex> vertices;
        std::vector<TransparentTriangle> transparent_triangles;

        // The indices are grouped by the number of textiles so that it can be drawn
        // as the selected texture.
        std::vector<std::vector<uint32_t>> indices(texture_storage.num_tiles());
        std::vector<uint32_t> untextured_indices;

        auto get_vertex = [&](std::size_t index, const trlevel::tr3_room& room)
        {
            auto v = room.data.vertices[index].vertex;
            return Vector3(v.x / 1024.f, -v.y / 1024.f, v.z / 1024.f);
        };

        for (const auto& rect : room.data.rectangles)
        {
            // What is selected inside the texture portion?
            //  The UV coordinates.
            //  Else, the face is a single colour.
            std::array<Vector2, 4> uvs = { Vector2(1,1), Vector2(1,1), Vector2(1,1), Vector2(1,1) };
            Color colour{ 1,1,1,1 };
            std::vector<uint32_t>* tex_indices_ptr = nullptr;

            std::array<Vector3, 4> verts;
            for (int i = 0; i < 4; ++i)
            {
                verts[i] = get_vertex(rect.vertices[i], room);
            }

            // Select UVs - otherwise they will be 0.
            const uint16_t texture = rect.texture & 0x7fff;
            for (int i = 0; i < uvs.size(); ++i)
            {
                uvs[i] = texture_storage.uv(texture, i);
            }

            bool double_sided = rect.texture & 0x8000;

            uint16_t attribute = texture_storage.attribute(texture);
            if (attribute != 0)
            {
                auto mode = attribute_to_transparency(attribute);
                transparent_triangles.emplace_back(verts[0], verts[1], verts[2], uvs[0], uvs[1], uvs[2], texture_storage.tile(texture), mode);
                transparent_triangles.emplace_back(verts[2], verts[3], verts[0], uvs[2], uvs[3], uvs[0], texture_storage.tile(texture), mode);
                if (double_sided)
                {
                    transparent_triangles.emplace_back(verts[2], verts[1], verts[0], uvs[2], uvs[1], uvs[0], texture_storage.tile(texture), mode);
                    transparent_triangles.emplace_back(verts[0], verts[3], verts[2], uvs[0], uvs[3], uvs[2], texture_storage.tile(texture), mode);
                }
                continue;
            }

            tex_indices_ptr = &indices[texture_storage.tile(texture)];

            auto base = vertices.size();
            for (int i = 0; i < 4; ++i)
            {
                vertices.push_back({verts[i], uvs[i], colour });
            }

            auto& tex_indices = *tex_indices_ptr;
            tex_indices.push_back(base);
            tex_indices.push_back(base + 1);
            tex_indices.push_back(base + 2);
            tex_indices.push_back(base + 2);
            tex_indices.push_back(base + 3);
            tex_indices.push_back(base + 0);

            if (double_sided)
            {
                tex_indices.push_back(base + 2);
                tex_indices.push_back(base + 1);
                tex_indices.push_back(base);
                tex_indices.push_back(base);
                tex_indices.push_back(base + 3);
                tex_indices.push_back(base + 2);
            }

            _collision_triangles.push_back(Triangle(vertices[base].pos, vertices[base + 1].pos, vertices[base + 2].pos));
            _collision_triangles.push_back(Triangle(vertices[base + 2].pos, vertices[base + 3].pos, vertices[base + 0].pos));
            if (double_sided)
            {
                _collision_triangles.push_back(Triangle(vertices[base + 2].pos, vertices[base + 1].pos, vertices[base + 0].pos));
                _collision_triangles.push_back(Triangle(vertices[base + 0].pos, vertices[base + 3].pos, vertices[base + 2].pos));
            }
        }

        for (const auto& tri : room.data.triangles)
        {
            // What is selected inside the texture portion?
            //  The UV coordinates.
            //  Else, the face is a single colour.
            std::array<Vector2, 3> uvs = { Vector2(1,1), Vector2(1,1), Vector2(1,1) };
            Color colour{ 1,1,1,1 };
            std::vector<uint32_t>* tex_indices_ptr = nullptr;
            std::array<Vector3, 3> verts;
            for (int i = 0; i < 3; ++i)
            {
                verts[i] = get_vertex(tri.vertices[i], room);
            }

            // Select UVs - otherwise they will be 0.
            const uint16_t texture = tri.texture & 0x7fff;
            for (int i = 0; i < uvs.size(); ++i)
            {
                uvs[i] = texture_storage.uv(texture, i);
            }

            bool double_sided = tri.texture & 0x8000;

            uint16_t attribute = texture_storage.attribute(texture);
            if (attribute != 0)
            {
                auto mode = attribute_to_transparency(attribute);
                transparent_triangles.emplace_back(verts[0], verts[1], verts[2], uvs[0], uvs[1], uvs[2], texture_storage.tile(texture), mode);
                if (double_sided)
                {
                    transparent_triangles.emplace_back(verts[2], verts[1], verts[0], uvs[2], uvs[1], uvs[0], texture_storage.tile(texture), mode);
                }
                continue;
            }

            tex_indices_ptr = &indices[texture_storage.tile(texture)];

            auto base = vertices.size();
            for (int i = 0; i < 3; ++i)
            {
                vertices.push_back({ verts[i], uvs[i], colour });
            }

            auto& tex_indices = *tex_indices_ptr;
            tex_indices.push_back(base);
            tex_indices.push_back(base + 1);
            tex_indices.push_back(base + 2);
            if (double_sided)
            {
                tex_indices.push_back(base + 2);
                tex_indices.push_back(base + 1);
                tex_indices.push_back(base);
            }

            _collision_triangles.push_back(Triangle(vertices[base].pos, vertices[base + 1].pos, vertices[base + 2].pos));
            if (double_sided)
            {
                _collision_triangles.push_back(Triangle(vertices[base + 2].pos, vertices[base + 1].pos, vertices[base].pos));
            }
        }

        _mesh = std::make_unique<Mesh>(_device, vertices, indices, untextured_indices, transparent_triangles, texture_storage);

        // Generate the bounding box for use in picking.
        Vector3 minimum(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector3 maximum(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (const auto& v : vertices)
        {
            minimum = Vector3::Min(minimum, v.pos);
            maximum = Vector3::Max(maximum, v.pos);
        }

        const Vector3 half_size = (maximum - minimum) * 0.5f;
        _bounding_box.Extents = half_size;
        _bounding_box.Center = minimum + half_size;
    }

    void Room::generate_adjacency(const trlevel::ILevel& level, const trlevel::tr3_room& room)
    {
        std::set<uint16_t> adjacent_rooms;
        for (const auto& sector : room.sector_list)
        {
            if (sector.room_above != 0xff)
            {
                adjacent_rooms.insert(sector.room_above);
            }

            if (sector.room_below != 0xff)
            {
                adjacent_rooms.insert(sector.room_below);
            }

            uint32_t index = sector.floordata_index;

            // There's no floordata for this sector.
            if (index == 0)
            {
                continue;
            }

            bool end_data = false;

            do
            {
                uint16_t floor_data = level.get_floor_data(index);
                end_data = floor_data & 0x8000;
                uint32_t function = floor_data & 0x001F;

                // Portal
                switch (function)
                {
                case 0x0:
                    end_data = true;
                    break;
                case 0x1:
                    adjacent_rooms.insert(level.get_floor_data(++index));
                    break;
                case 0x2:
                case 0x3:
                    ++index;
                    break;
                case 0x4:
                {
                    uint16_t trigger_setup = level.get_floor_data(++index);
                    uint16_t action = 0;
                    do
                    {
                        uint16_t action = level.get_floor_data(++index);
                    } while (action & 0x8000);
                    break;
                }
                case 0x5:
                case 0x6:
                    break;
                case 0x7:
                case 0x8:
                case 0x9:
                case 0xA:
                case 0xB:
                case 0xC:
                case 0xD:
                case 0xE:
                case 0xF:
                case 0x10:
                case 0x11:
                case 0x12:
                {
                    uint16_t triangulation = level.get_floor_data(++index);
                    break;
                }
                case 0x13:
                    break;
                case 0x14: // trigger triggerer
                    break;
                case 0x15:
                    break;
                }

                ++index;

            } while (!end_data);
            
        }

        // Above and below.
        _neighbours = adjacent_rooms;
    }

    void Room::add_entity(Entity* entity)
    {
        _entities.push_back(entity);
    }

    void Room::get_transparent_triangles(TransparencyBuffer& transparency, const ICamera& camera, SelectionMode selected)
    {
        using namespace DirectX::SimpleMath;
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

        for (const auto& entity : _entities)
        {
            entity->get_transparent_triangles(transparency, camera, colour);
        }
    }
}

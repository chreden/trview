#include "stdafx.h"
#include "Room.h"
#include "RoomVertex.h"

#include "ILevelTextureStorage.h"
#include "IMeshStorage.h"

#include <directxmath.h>
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
        using namespace DirectX;
        _room_offset = XMMatrixTranslation(room.info.x / 1024.f, 0, room.info.z / 1024.f);

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

    Room::PickResult Room::pick(DirectX::XMVECTOR position, DirectX::XMVECTOR direction)
    {
        using namespace DirectX;
        using namespace DirectX::TriangleTests;

        PickResult result;

        auto room_offset = XMMatrixTranslation(-_info.x / 1024.f, 0, -_info.z / 1024.f);
        auto transformed_position = XMVector3TransformCoord(position, room_offset);

        bool any_hit = false;
        result.distance = FLT_MAX;
        for (const auto& tri : _collision_triangles)
        {
            float distance = 0;
            if (XMVectorGetX(XMVector3Dot(direction, tri.normal)) < 0 &&
                Intersects(transformed_position, direction, tri.v0, tri.v1, tri.v2, distance))
            {
                result.hit = true;
                if (distance < result.distance)
                {
                    result.distance = distance;
                }
            }
        }

        // Calculate the world space hit position, if there was a hit.
        if (result.hit)
        {
            result.position = XMVectorAdd(position, XMVectorScale(direction, result.distance));
        }

        return result;
    }

    void Room::render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& view_projection, const ILevelTextureStorage& texture_storage, SelectionMode selected)
    {
        // There are no vertices.
        if (!_vertex_buffer)
        {
            return;
        }

        using namespace DirectX;

        auto wvp = _room_offset * view_projection;

        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        memset(&mapped_resource, 0, sizeof(mapped_resource));

        struct Data
        {
            DirectX::XMMATRIX matrix;
            DirectX::XMFLOAT4 colour;
        };

        XMFLOAT4 colour = selected == SelectionMode::Selected ? XMFLOAT4( 1, 1, 1, 1) : 
                          selected == SelectionMode::Neighbour ? XMFLOAT4(0.4f, 0.4f, 0.4f, 1) : XMFLOAT4(0.2f, 0.2f, 0.2f, 1 );
        Data data{ wvp, colour };

        context->Map(_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
        memcpy(mapped_resource.pData, &data, sizeof(data));
        context->Unmap(_matrix_buffer, 0);

        UINT stride = sizeof(room_vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &_vertex_buffer.p, &stride, &offset);
        context->VSSetConstantBuffers(0, 1, &_matrix_buffer.p);

        for (uint32_t i = 0; i < _index_buffers.size(); ++i)
        {
            auto& index_buffer = _index_buffers[i];
            if (index_buffer)
            {
                auto texture = texture_storage.texture(i);
                context->PSSetShaderResources(0, 1, &texture.view.p);
                context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
                context->DrawIndexed(_index_counts[i], 0, 0);
            }
        }

        if (_untextured_index_count)
        {
            auto texture = texture_storage.untextured();
            context->PSSetShaderResources(0, 1, &texture.view.p);
            context->IASetIndexBuffer(_untextured_index_buffer, DXGI_FORMAT_R32_UINT, 0);
            context->DrawIndexed(_untextured_index_count, 0, 0);
        }

        for (const auto& mesh : _static_meshes)
        {
            mesh->render(context, view_projection, texture_storage);
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
        using namespace DirectX;

        // Geometry.
        std::vector<room_vertex> vertices;

        // The indices are grouped by the number of textiles so that it can be drawn
        // as the selected texture.
        std::vector<std::vector<uint32_t>> indices(level.num_textiles());
        std::vector<uint32_t> untextured_indices;

        auto get_vertex = [&](std::size_t index, const trlevel::tr3_room& room)
        {
            auto v = room.data.vertices[index].vertex;
            return XMFLOAT3(v.x / 1024.f, -v.y / 1024.f, v.z / 1024.f);
        };

        for (const auto& rect : room.data.rectangles)
        {
            // What is selected inside the texture portion?
            //  The UV coordinates.
            //  Else, the face is a single colour.
            std::array<XMFLOAT2, 4> uvs = { XMFLOAT2{ 1,1 }, XMFLOAT2{ 1,1 }, XMFLOAT2{ 1,1 }, XMFLOAT2{ 1,1 } };
            XMFLOAT4 colour{ 1,1,1,1 };
            std::vector<uint32_t>* tex_indices_ptr = nullptr;

            // Select UVs - otherwise they will be 0.
            if (rect.texture < level.num_object_textures())
            {
                for (int i = 0; i < uvs.size(); ++i)
                {
                    uvs[i] = texture_storage.uv(rect.texture, i);
                }
                tex_indices_ptr = &indices[texture_storage.tile(rect.texture)];
            }
            else
            {
                tex_indices_ptr = &untextured_indices;
                colour = texture_storage.palette_from_texture(rect.texture);
            }

            auto base = vertices.size();
            for (int i = 0; i < 4; ++i)
            {
                vertices.push_back({ get_vertex(rect.vertices[i], room), uvs[i], colour });
            }

            auto& tex_indices = *tex_indices_ptr;
            tex_indices.push_back(base);
            tex_indices.push_back(base + 1);
            tex_indices.push_back(base + 2);
            tex_indices.push_back(base + 2);
            tex_indices.push_back(base + 3);
            tex_indices.push_back(base + 0);

            _collision_triangles.push_back(Triangle(XMLoadFloat3(&vertices[base].pos), XMLoadFloat3(&vertices[base + 1].pos), XMLoadFloat3(&vertices[base + 2].pos)));
            _collision_triangles.push_back(Triangle(XMLoadFloat3(&vertices[base + 2].pos), XMLoadFloat3(&vertices[base + 3].pos), XMLoadFloat3(&vertices[base + 0].pos)));
        }

        for (const auto& tri : room.data.triangles)
        {
            // What is selected inside the texture portion?
            //  The UV coordinates.
            //  Else, the face is a single colour.
            std::array<XMFLOAT2, 3> uvs = { XMFLOAT2{ 1,1 }, XMFLOAT2{ 1,1 }, XMFLOAT2{ 1,1 } };
            XMFLOAT4 colour{ 1,1,1,1 };
            std::vector<uint32_t>* tex_indices_ptr = nullptr;

            // Select UVs - otherwise they will be 0.
            if (tri.texture < level.num_object_textures())
            {
                for (int i = 0; i < uvs.size(); ++i)
                {
                    uvs[i] = texture_storage.uv(tri.texture, i);
                }
                tex_indices_ptr = &indices[texture_storage.tile(tri.texture)];
            }
            else
            {
                tex_indices_ptr = &untextured_indices;
                colour = texture_storage.palette_from_texture(tri.texture);
            }

            auto base = vertices.size();
            for (int i = 0; i < 3; ++i)
            {
                vertices.push_back({ get_vertex(tri.vertices[i], room), uvs[i], colour });
            }

            auto& tex_indices = *tex_indices_ptr;
            tex_indices.push_back(base);
            tex_indices.push_back(base + 1);
            tex_indices.push_back(base + 2);

            _collision_triangles.push_back(Triangle(XMLoadFloat3(&vertices[base].pos), XMLoadFloat3(&vertices[base + 1].pos), XMLoadFloat3(&vertices[base + 2].pos)));
        }

        if (!vertices.empty())
        {
            D3D11_BUFFER_DESC vertex_desc;
            memset(&vertex_desc, 0, sizeof(vertex_desc));
            vertex_desc.Usage = D3D11_USAGE_DEFAULT;
            vertex_desc.ByteWidth = sizeof(room_vertex) * vertices.size();
            vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA vertex_data;
            memset(&vertex_data, 0, sizeof(vertex_data));
            vertex_data.pSysMem = &vertices[0];

            HRESULT hr = _device->CreateBuffer(&vertex_desc, &vertex_data, &_vertex_buffer);

            for (const auto& tex_indices : indices)
            {
                _index_counts.push_back(tex_indices.size());

                if (!tex_indices.size())
                {
                    _index_buffers.push_back(nullptr);
                    continue;
                }

                D3D11_BUFFER_DESC index_desc;
                memset(&index_desc, 0, sizeof(index_desc));
                index_desc.Usage = D3D11_USAGE_DEFAULT;
                index_desc.ByteWidth = sizeof(uint32_t) * tex_indices.size();
                index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

                D3D11_SUBRESOURCE_DATA index_data;
                memset(&index_data, 0, sizeof(index_data));
                index_data.pSysMem = &tex_indices[0];

                CComPtr<ID3D11Buffer> index_buffer;
                hr = _device->CreateBuffer(&index_desc, &index_data, &index_buffer);
                _index_buffers.push_back(index_buffer);
            }

            if (!untextured_indices.empty())
            {
                D3D11_BUFFER_DESC index_desc;
                memset(&index_desc, 0, sizeof(index_desc));
                index_desc.Usage = D3D11_USAGE_DEFAULT;
                index_desc.ByteWidth = sizeof(uint32_t) * untextured_indices.size();
                index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

                D3D11_SUBRESOURCE_DATA index_data;
                memset(&index_data, 0, sizeof(index_data));
                index_data.pSysMem = &untextured_indices[0];

                CComPtr<ID3D11Buffer> index_buffer;
                hr = _device->CreateBuffer(&index_desc, &index_data, &_untextured_index_buffer);
                _untextured_index_count = untextured_indices.size();
            }

            D3D11_BUFFER_DESC matrix_desc;
            memset(&matrix_desc, 0, sizeof(matrix_desc));

            matrix_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            matrix_desc.ByteWidth = sizeof(DirectX::XMMATRIX) + sizeof(DirectX::XMFLOAT4);
            matrix_desc.Usage = D3D11_USAGE_DYNAMIC;
            matrix_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            _device->CreateBuffer(&matrix_desc, nullptr, &_matrix_buffer);
        }
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
}

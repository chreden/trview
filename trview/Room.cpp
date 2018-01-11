#include "stdafx.h"
#include "Room.h"
#include "RoomVertex.h"

#include <directxmath.h>
#include <array>

namespace trview
{
    Room::Room(CComPtr<ID3D11Device> device, const trlevel::ILevel& level, const trlevel::tr3_room& room)
        : _device(device), _info { room.info.x, 0, room.info.z, room.info.yBottom, room.info.yTop }
    {
        using namespace DirectX;

        _room_offset = XMMatrixTranslation(room.info.x / 1024.f, 0, room.info.z / 1024.f);

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

        auto get_uv = [&](const trlevel::tr_object_texture& texture, std::size_t index)
        {
            // Find the the point in the object map.
            auto vert = texture.Vertices[index];
            return XMFLOAT2(vert.Xpixel / 255.0f, vert.Ypixel / 255.0f);
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
                auto texture = level.get_object_texture(rect.texture);
                for (int i = 0; i < uvs.size(); ++i)
                {
                    uvs[i] = get_uv(texture, i);
                }
                tex_indices_ptr = &indices[texture.TileAndFlag & 0x7fff];
            }
            else
            {
                tex_indices_ptr = &untextured_indices;
                auto palette_index = (rect.texture & 0x7FFF) >> 8;
                auto palette = level.get_palette_entry_16(palette_index);
                colour = XMFLOAT4(palette.Red / 255.f, palette.Green / 255.f, palette.Blue / 255.f, 1.0f);
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
                auto texture = level.get_object_texture(tri.texture);
                for (int i = 0; i < uvs.size(); ++i)
                {
                    uvs[i] = get_uv(texture, i);
                }
                tex_indices_ptr = &indices[texture.TileAndFlag & 0x7fff];
            }
            else
            {
                tex_indices_ptr = &untextured_indices;
                auto palette_index = (tri.texture & 0x7FFF) >> 8;
                auto palette = level.get_palette_entry_16(palette_index);
                colour = XMFLOAT4(palette.Red / 255.f, palette.Green / 255.f, palette.Blue / 255.f, 1.0f);
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

            device->CreateBuffer(&matrix_desc, nullptr, &_matrix_buffer);

            uint32_t pixel = 0xffffffff;
            D3D11_SUBRESOURCE_DATA srd;
            memset(&srd, 0, sizeof(srd));
            srd.pSysMem = &pixel;
            srd.SysMemPitch = sizeof(uint32_t);

            D3D11_TEXTURE2D_DESC tex_desc;
            memset(&tex_desc, 0, sizeof(tex_desc));
            tex_desc.Width = 1;
            tex_desc.Height = 1;
            tex_desc.MipLevels = tex_desc.ArraySize = 1;
            tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            tex_desc.SampleDesc.Count = 1;
            tex_desc.Usage = D3D11_USAGE_DEFAULT;
            tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            tex_desc.CPUAccessFlags = 0;
            tex_desc.MiscFlags = 0;

            device->CreateTexture2D(&tex_desc, &srd, &_untextured_texture.texture);
            device->CreateShaderResourceView(_untextured_texture.texture, nullptr, &_untextured_texture.view);
        }
    }

    RoomInfo Room::info() const
    {
        return _info;
    }

    void Room::render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& view_projection, std::vector<Texture>& level_textures, bool selected)
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

        XMFLOAT4 colour = selected ? XMFLOAT4( 1, 1, 1, 1) : XMFLOAT4(0.2f, 0.2f, 0.2f, 1 );
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
                context->PSSetShaderResources(0, 1, &level_textures[i].view.p);
                context->IASetIndexBuffer(index_buffer, DXGI_FORMAT_R32_UINT, 0);
                context->DrawIndexed(_index_counts[i], 0, 0);
            }
        }

        if (_untextured_index_count)
        {
            context->PSSetShaderResources(0, 1, &_untextured_texture.view.p);
            context->IASetIndexBuffer(_untextured_index_buffer, DXGI_FORMAT_R32_UINT, 0);
            context->DrawIndexed(_untextured_index_count, 0, 0);
        }
    }
}

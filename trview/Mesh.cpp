#include "stdafx.h"
#include "Mesh.h"

#include <array>

#include "ILevelTextureStorage.h"

namespace trview
{
    namespace
    {
        struct mesh_vertex
        {
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT2 uv;
            DirectX::XMFLOAT4 colour;
        };
    }

    Mesh::Mesh(const trlevel::tr_mesh& mesh, CComPtr<ID3D11Device> device, const ILevelTextureStorage& texture_storage)
    {
        using namespace DirectX;

        std::vector<std::vector<uint32_t>> indices(texture_storage.num_tiles());
        std::vector<mesh_vertex> vertices;
        std::vector<uint32_t> untextured_indices;

        auto get_vertex = [&](std::size_t index, const trlevel::tr_mesh& mesh)
        {
            auto v = mesh.vertices[index];
            return XMFLOAT3(v.x / 1024.f, -v.y / 1024.f, v.z / 1024.f);
        };

        for (const auto& rect : mesh.textured_rectangles)
        {
            const XMFLOAT4 colour{ 1,1,1,1 };

            std::array<XMFLOAT2, 4> uvs = 
            { 
                texture_storage.uv(rect.texture, 0), 
                texture_storage.uv(rect.texture, 1),
                texture_storage.uv(rect.texture, 2),
                texture_storage.uv(rect.texture, 3)
            };

            std::vector<uint32_t>* tex_indices_ptr = &indices[texture_storage.tile(rect.texture)];
            auto base = vertices.size();
            for (int i = 0; i < 4; ++i)
            {
                vertices.push_back({ get_vertex(rect.vertices[i], mesh), uvs[i], colour });
            }

            auto& tex_indices = *tex_indices_ptr;
            tex_indices.push_back(base);
            tex_indices.push_back(base + 1);
            tex_indices.push_back(base + 2);
            tex_indices.push_back(base + 2);
            tex_indices.push_back(base + 3);
            tex_indices.push_back(base + 0);
        }

        for (const auto& tri : mesh.textured_triangles)
        {
            const XMFLOAT4 colour{ 1,1,1,1 };

            std::array<XMFLOAT2, 3> uvs =
            {
                texture_storage.uv(tri.texture, 0),
                texture_storage.uv(tri.texture, 1),
                texture_storage.uv(tri.texture, 2),
            };

            std::vector<uint32_t>* tex_indices_ptr = &indices[texture_storage.tile(tri.texture)];
            auto base = vertices.size();
            for (int i = 0; i < 3; ++i)
            {
                vertices.push_back({ get_vertex(tri.vertices[i], mesh), uvs[i], colour });
            }

            auto& tex_indices = *tex_indices_ptr;
            tex_indices.push_back(base);
            tex_indices.push_back(base + 1);
            tex_indices.push_back(base + 2);
        }

        for (const auto& rect : mesh.coloured_rectangles)
        {
            auto base = vertices.size();
            for (int i = 0; i < 4; ++i)
            {
                vertices.push_back({ get_vertex(rect.vertices[i], mesh), XMFLOAT2{ 0,0 }, texture_storage.palette_from_texture(rect.texture)});
            }

            untextured_indices.push_back(base);
            untextured_indices.push_back(base + 1);
            untextured_indices.push_back(base + 2);
            untextured_indices.push_back(base + 2);
            untextured_indices.push_back(base + 3);
            untextured_indices.push_back(base + 0);
        }

        for (const auto& tri : mesh.coloured_triangles)
        {
            auto base = vertices.size();
            for (int i = 0; i < 3; ++i)
            {
                vertices.push_back({ get_vertex(tri.vertices[i], mesh), XMFLOAT2{ 0,0 }, texture_storage.palette_from_texture(tri.texture) });
            }

            untextured_indices.push_back(base);
            untextured_indices.push_back(base + 1);
            untextured_indices.push_back(base + 2);
        }

        if (!vertices.empty())
        {
            D3D11_BUFFER_DESC vertex_desc;
            memset(&vertex_desc, 0, sizeof(vertex_desc));
            vertex_desc.Usage = D3D11_USAGE_DEFAULT;
            vertex_desc.ByteWidth = sizeof(mesh_vertex) * vertices.size();
            vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA vertex_data;
            memset(&vertex_data, 0, sizeof(vertex_data));
            vertex_data.pSysMem = &vertices[0];

            HRESULT hr = device->CreateBuffer(&vertex_desc, &vertex_data, &_vertex_buffer);

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
                hr = device->CreateBuffer(&index_desc, &index_data, &index_buffer);
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
                hr = device->CreateBuffer(&index_desc, &index_data, &_untextured_index_buffer);
                _untextured_index_count = untextured_indices.size();
            }

            D3D11_BUFFER_DESC matrix_desc;
            memset(&matrix_desc, 0, sizeof(matrix_desc));

            matrix_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            matrix_desc.ByteWidth = sizeof(DirectX::XMMATRIX) + sizeof(DirectX::XMFLOAT4);
            matrix_desc.Usage = D3D11_USAGE_DYNAMIC;
            matrix_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            device->CreateBuffer(&matrix_desc, nullptr, &_matrix_buffer);
        }

    }

    void Mesh::render(CComPtr<ID3D11DeviceContext> context, const DirectX::XMMATRIX& world_view_projection, const ILevelTextureStorage& texture_storage, const DirectX::XMFLOAT4& colour)
    {
        // There are no vertices.
        if (!_vertex_buffer)
        {
            return;
        }

        using namespace DirectX;

        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        memset(&mapped_resource, 0, sizeof(mapped_resource));

        struct Data
        {
            DirectX::XMMATRIX matrix;
            DirectX::XMFLOAT4 colour;
        };

        Data data{ world_view_projection, colour };

        context->Map(_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
        memcpy(mapped_resource.pData, &data, sizeof(data));
        context->Unmap(_matrix_buffer, 0);

        UINT stride = sizeof(mesh_vertex);
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
    }
}
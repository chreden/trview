#define NOMINMAX

#include "Mesh.h"

#include <array>
#include <DirectXCollision.h>

#include "ILevelTextureStorage.h"

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        // Convert the vertex to the scale used by the viewer.
        // vertex: The vertex to convert.
        // Returns: The scaled vector.
        Vector3 convert_vertex(const trlevel::tr_vertex& vertex)
        {
            return Vector3(vertex.x / 1024.f, -vertex.y / 1024.f, vertex.z / 1024.f);
        };
    }

    Mesh::Mesh(const ComPtr<ID3D11Device>& device, 
        const std::vector<MeshVertex>& vertices, 
        const std::vector<std::vector<uint32_t>>& indices, 
        const std::vector<uint32_t>& untextured_indices, 
        const std::vector<TransparentTriangle>& transparent_triangles,
        const std::vector<Triangle>& collision_triangles)
        : _transparent_triangles(transparent_triangles), _collision_triangles(collision_triangles)
    {
        if (!vertices.empty())
        {
            D3D11_BUFFER_DESC vertex_desc;
            memset(&vertex_desc, 0, sizeof(vertex_desc));
            vertex_desc.Usage = D3D11_USAGE_DEFAULT;
            vertex_desc.ByteWidth = sizeof(MeshVertex) * static_cast<uint32_t>(vertices.size());
            vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

            D3D11_SUBRESOURCE_DATA vertex_data;
            memset(&vertex_data, 0, sizeof(vertex_data));
            vertex_data.pSysMem = &vertices[0];

            HRESULT hr = device->CreateBuffer(&vertex_desc, &vertex_data, &_vertex_buffer);

            for (const auto& tex_indices : indices)
            {
                _index_counts.push_back(static_cast<uint32_t>(tex_indices.size()));

                if (!tex_indices.size())
                {
                    _index_buffers.push_back(nullptr);
                    continue;
                }

                D3D11_BUFFER_DESC index_desc;
                memset(&index_desc, 0, sizeof(index_desc));
                index_desc.Usage = D3D11_USAGE_DEFAULT;
                index_desc.ByteWidth = sizeof(uint32_t) * static_cast<uint32_t>(tex_indices.size());
                index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

                D3D11_SUBRESOURCE_DATA index_data;
                memset(&index_data, 0, sizeof(index_data));
                index_data.pSysMem = &tex_indices[0];

                ComPtr<ID3D11Buffer> index_buffer;
                hr = device->CreateBuffer(&index_desc, &index_data, &index_buffer);
                _index_buffers.push_back(index_buffer);
            }

            if (!untextured_indices.empty())
            {
                D3D11_BUFFER_DESC index_desc;
                memset(&index_desc, 0, sizeof(index_desc));
                index_desc.Usage = D3D11_USAGE_DEFAULT;
                index_desc.ByteWidth = sizeof(uint32_t) * static_cast<uint32_t>(untextured_indices.size());
                index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

                D3D11_SUBRESOURCE_DATA index_data;
                memset(&index_data, 0, sizeof(index_data));
                index_data.pSysMem = &untextured_indices[0];

                ComPtr<ID3D11Buffer> index_buffer;
                hr = device->CreateBuffer(&index_desc, &index_data, &_untextured_index_buffer);
                _untextured_index_count = static_cast<uint32_t>(untextured_indices.size());
            }

            D3D11_BUFFER_DESC matrix_desc;
            memset(&matrix_desc, 0, sizeof(matrix_desc));

            matrix_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            matrix_desc.ByteWidth = sizeof(Matrix) + sizeof(Color);
            matrix_desc.Usage = D3D11_USAGE_DYNAMIC;
            matrix_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            device->CreateBuffer(&matrix_desc, nullptr, &_matrix_buffer);
        }

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

    void Mesh::render(const ComPtr<ID3D11DeviceContext>& context, const Matrix& world_view_projection, const ILevelTextureStorage& texture_storage, const Color& colour)
    {
        // There are no vertices.
        if (!_vertex_buffer)
        {
            return;
        }

        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        memset(&mapped_resource, 0, sizeof(mapped_resource));

        struct Data
        {
            Matrix matrix;
            Color colour;
        };

        Data data{ world_view_projection, colour };

        context->Map(_matrix_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
        memcpy(mapped_resource.pData, &data, sizeof(data));
        context->Unmap(_matrix_buffer.Get(), 0);

        UINT stride = sizeof(MeshVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, _vertex_buffer.GetAddressOf(), &stride, &offset);
        context->VSSetConstantBuffers(0, 1, _matrix_buffer.GetAddressOf());

        for (uint32_t i = 0; i < _index_buffers.size(); ++i)
        {
            auto& index_buffer = _index_buffers[i];
            if (index_buffer)
            {
                auto texture = texture_storage.texture(i);
                context->PSSetShaderResources(0, 1, texture.view().GetAddressOf());
                context->IASetIndexBuffer(index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                context->DrawIndexed(_index_counts[i], 0, 0);
            }
        }

        if (_untextured_index_count)
        {
            auto texture = texture_storage.untextured();
            context->PSSetShaderResources(0, 1, texture.view().GetAddressOf());
            context->IASetIndexBuffer(_untextured_index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            context->DrawIndexed(_untextured_index_count, 0, 0);
        }
    }

    const std::vector<TransparentTriangle>& Mesh::transparent_triangles() const
    {
        return _transparent_triangles;
    }

    const DirectX::BoundingBox& Mesh::bounding_box() const
    {
        return _bounding_box;
    }

    PickResult Mesh::pick(const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& direction) const
    {
        using namespace DirectX::TriangleTests;

        PickResult result;
        result.type = PickResult::Type::Mesh;
        for (const auto& tri : _collision_triangles)
        {
            float distance = 0;
            if (direction.Dot(tri.normal) < 0 &&
                Intersects(position, direction, tri.v0, tri.v1, tri.v2, distance))
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

    std::unique_ptr<Mesh> create_mesh(const trlevel::tr_mesh& mesh, const ComPtr<ID3D11Device>& device, const ILevelTextureStorage& texture_storage, bool transparent_collision)
    {
        std::vector<std::vector<uint32_t>> indices(texture_storage.num_tiles());
        std::vector<MeshVertex> vertices;
        std::vector<uint32_t> untextured_indices;
        std::vector<TransparentTriangle> transparent_triangles;
        std::vector<Triangle> collision_triangles;

        process_textured_rectangles(mesh.textured_rectangles, mesh.vertices, texture_storage, vertices, indices, transparent_triangles, collision_triangles, transparent_collision);
        process_textured_triangles(mesh.textured_triangles, mesh.vertices, texture_storage, vertices, indices, transparent_triangles, collision_triangles, transparent_collision);
        process_coloured_rectangles(mesh.coloured_rectangles, mesh.vertices, texture_storage, vertices, untextured_indices, collision_triangles);
        process_coloured_triangles(mesh.coloured_triangles, mesh.vertices, texture_storage, vertices, untextured_indices, collision_triangles);

        return std::make_unique<Mesh>(device, vertices, indices, untextured_indices, transparent_triangles, collision_triangles);
    }

    void process_textured_rectangles(
        const std::vector<trlevel::tr4_mesh_face4>& rectangles, 
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<std::vector<uint32_t>>& output_indices,
        std::vector<TransparentTriangle>& transparent_triangles,
        std::vector<Triangle>& collision_triangles,
        bool transparent_collision)
    {
        using namespace trlevel;

        for (const auto& rect : rectangles)
        {
            std::array<Vector3, 4> verts;
            for (int i = 0; i < 4; ++i)
            {
                verts[i] = convert_vertex(input_vertices[rect.vertices[i]]);
            }

            const uint16_t texture = rect.texture & 0x7fff;

            std::array<Vector2, 4> uvs;
            for (int i = 0; i < uvs.size(); ++i)
            {
                uvs[i] = texture_storage.uv(texture, i);
            }

            const bool double_sided = rect.texture & 0x8000;

            TransparentTriangle::Mode transparency_mode;
            if (determine_transparency(texture_storage.attribute(texture), rect.effects, transparency_mode))
            {
                transparent_triangles.emplace_back(verts[0], verts[1], verts[2], uvs[0], uvs[1], uvs[2], texture_storage.tile(texture), transparency_mode);
                transparent_triangles.emplace_back(verts[2], verts[3], verts[0], uvs[2], uvs[3], uvs[0], texture_storage.tile(texture), transparency_mode);
                if (transparent_collision)
                {
                    collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
                    collision_triangles.emplace_back(verts[2], verts[3], verts[0]);
                }
            
                if (double_sided)
                {
                    
                    transparent_triangles.emplace_back(verts[2], verts[1], verts[0], uvs[2], uvs[1], uvs[0], texture_storage.tile(texture), transparency_mode);
                    transparent_triangles.emplace_back(verts[0], verts[3], verts[2], uvs[0], uvs[3], uvs[2], texture_storage.tile(texture), transparency_mode);
                    if (transparent_collision)
                    {
                        collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
                        collision_triangles.emplace_back(verts[0], verts[3], verts[2]);
                    }
                }
                continue;
            }

            const uint32_t base = static_cast<uint32_t>(output_vertices.size());
            for (int i = 0; i < 4; ++i)
            {
                output_vertices.push_back({ verts[i], uvs[i], Color(1,1,1,1) });
            }

            auto& tex_indices = output_indices[texture_storage.tile(texture)];
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

            collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
            collision_triangles.emplace_back(verts[2], verts[3], verts[0]);
            if (double_sided)
            {
                collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
                collision_triangles.emplace_back(verts[0], verts[3], verts[2]);
            }
        }
    }

    void process_textured_triangles(
        const std::vector<trlevel::tr4_mesh_face3>& triangles,
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<std::vector<uint32_t>>& output_indices,
        std::vector<TransparentTriangle>& transparent_triangles,
        std::vector<Triangle>& collision_triangles,
        bool transparent_collision)
    {
        for (const auto& tri : triangles)
        {
            std::array<Vector3, 3> verts;
            for (int i = 0; i < 3; ++i)
            {
                verts[i] = convert_vertex(input_vertices[tri.vertices[i]]);
            }

            // Select UVs - otherwise they will be 0.
            const uint16_t texture = tri.texture & 0x7fff;
            std::array<Vector2, 3> uvs;
            for (int i = 0; i < uvs.size(); ++i)
            {
                uvs[i] = texture_storage.uv(texture, i);
            }

            const bool double_sided = tri.texture & 0x8000;

            TransparentTriangle::Mode transparency_mode;
            if (determine_transparency(texture_storage.attribute(texture), tri.effects, transparency_mode))
            {
                transparent_triangles.emplace_back(verts[0], verts[1], verts[2], uvs[0], uvs[1], uvs[2], texture_storage.tile(texture), transparency_mode);
                if (transparent_collision)
                {
                    collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
                }
                if (double_sided)
                {
                    transparent_triangles.emplace_back(verts[2], verts[1], verts[0], uvs[2], uvs[1], uvs[0], texture_storage.tile(texture), transparency_mode);
                    if (transparent_collision)
                    {
                        collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
                    }
                }
                continue;
            }

            const uint32_t base = static_cast<uint32_t>(output_vertices.size());
            for (int i = 0; i < 3; ++i)
            {
                output_vertices.push_back({ verts[i], uvs[i], Color(1,1,1,1) });
            }

            auto& tex_indices = output_indices[texture_storage.tile(texture)];
            tex_indices.push_back(base);
            tex_indices.push_back(base + 1);
            tex_indices.push_back(base + 2);
            if (double_sided)
            {
                tex_indices.push_back(base + 2);
                tex_indices.push_back(base + 1);
                tex_indices.push_back(base);
            }

            collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
            if (double_sided)
            {
                collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
            }
        }
    }

    void process_coloured_rectangles(
        const std::vector<trlevel::tr_face4>& rectangles,
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<uint32_t>& output_indices,
        std::vector<Triangle>& collision_triangles)
    {
        for (const auto& rect : rectangles)
        {
            const uint16_t texture = rect.texture & 0x7fff;
            const bool double_sided = rect.texture & 0x8000;

            std::array<Vector3, 4> verts;
            for (int i = 0; i < 4; ++i)
            {
                verts[i] = convert_vertex(input_vertices[rect.vertices[i]]);
            }

            const uint32_t base = static_cast<uint32_t>(output_vertices.size());
            for (int i = 0; i < 4; ++i)
            {
                output_vertices.push_back({ verts[i], Vector2::Zero, texture_storage.palette_from_texture(texture) });
            }

            output_indices.push_back(base);
            output_indices.push_back(base + 1);
            output_indices.push_back(base + 2);
            output_indices.push_back(base + 2);
            output_indices.push_back(base + 3);
            output_indices.push_back(base + 0);
            if (double_sided)
            {
                output_indices.push_back(base + 2);
                output_indices.push_back(base + 1);
                output_indices.push_back(base);
                output_indices.push_back(base);
                output_indices.push_back(base + 3);
                output_indices.push_back(base + 2);
            }

            collision_triangles.emplace_back(verts[0], verts[1], verts[2]);
            collision_triangles.emplace_back(verts[2], verts[3], verts[0]);
            if (double_sided)
            {
                collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
                collision_triangles.emplace_back(verts[0], verts[3], verts[2]);
            }
        }
    }

    void process_coloured_triangles(
        const std::vector<trlevel::tr_face3>& triangles,
        const std::vector<trlevel::tr_vertex>& input_vertices,
        const ILevelTextureStorage& texture_storage,
        std::vector<MeshVertex>& output_vertices,
        std::vector<uint32_t>& output_indices,
        std::vector<Triangle>& collision_triangles)
    {
        for (const auto& tri : triangles)
        {
            const uint16_t texture = tri.texture & 0x7fff;
            const bool double_sided = tri.texture & 0x8000;

            std::array<Vector3, 3> verts;
            for (int i = 0; i < 3; ++i)
            {
                verts[i] = convert_vertex(input_vertices[tri.vertices[i]]);
            }

            const uint32_t base = static_cast<uint32_t>(output_vertices.size());
            for (int i = 0; i < 3; ++i)
            {
                output_vertices.push_back({ verts[i], Vector2::Zero, texture_storage.palette_from_texture(texture) });
            }

            output_indices.push_back(base);
            output_indices.push_back(base + 1);
            output_indices.push_back(base + 2);
            collision_triangles.emplace_back(verts[0], verts[1], verts[2]);

            if (double_sided)
            {
                output_indices.push_back(base + 2);
                output_indices.push_back(base + 1);
                output_indices.push_back(base);
                collision_triangles.emplace_back(verts[2], verts[1], verts[0]);
            }
        }
    }
}
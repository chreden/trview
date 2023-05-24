#include "Mesh.h"
#include <trview.app/Graphics/ILevelTextureStorage.h>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    Mesh::Mesh(const std::shared_ptr<graphics::IDevice>& device,
        const std::vector<MeshVertex>& vertices, 
        const std::vector<std::vector<uint32_t>>& indices, 
        const std::vector<uint32_t>& untextured_indices, 
        const std::vector<TransparentTriangle>& transparent_triangles,
        const std::vector<Triangle>& collision_triangles)
        : _device(device), _transparent_triangles(transparent_triangles), _collision_triangles(collision_triangles)
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

            _vertex_buffer = device->create_buffer(vertex_desc, vertex_data);

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

                _index_buffers.push_back(device->create_buffer(index_desc, index_data));
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

                _untextured_index_buffer = device->create_buffer(index_desc, index_data);
                _untextured_index_count = static_cast<uint32_t>(untextured_indices.size());
            }

            D3D11_BUFFER_DESC matrix_desc;
            memset(&matrix_desc, 0, sizeof(matrix_desc));

            matrix_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            matrix_desc.ByteWidth = sizeof(MeshData);
            matrix_desc.Usage = D3D11_USAGE_DYNAMIC;
            matrix_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            _matrix_buffer = device->create_buffer(matrix_desc, std::optional<D3D11_SUBRESOURCE_DATA>());
        }

        // Generate the bounding box for use in picking.
        calculate_bounding_box(vertices, transparent_triangles);
    }

    Mesh::Mesh(const std::vector<TransparentTriangle>& transparent_triangles, const std::vector<Triangle>& collision_triangles)
        : _transparent_triangles(transparent_triangles), _collision_triangles(collision_triangles)
    {
        calculate_bounding_box({}, transparent_triangles);
    }

    void Mesh::calculate_bounding_box(const std::vector<MeshVertex>& vertices, const std::vector<TransparentTriangle>& transparent_triangles)
    {
        Vector3 minimum(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector3 maximum(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (const auto& v : vertices)
        {
            minimum = Vector3::Min(minimum, v.pos);
            maximum = Vector3::Max(maximum, v.pos);
        }

        for (const auto& t : transparent_triangles)
        {
            for (const auto& v : t.vertices)
            {
                minimum = Vector3::Min(minimum, v);
                maximum = Vector3::Max(maximum, v);
            }
        }

        const bool no_vertices = vertices.empty() && transparent_triangles.empty();
        const Vector3 half_size = no_vertices ? Vector3::Zero : (maximum - minimum) * 0.5f;
        minimum = no_vertices ? Vector3::Zero : minimum;

        _bounding_box.Extents = half_size;
        _bounding_box.Center = minimum + half_size;
    }

    void Mesh::render(const Matrix& world_view_projection, const ILevelTextureStorage& texture_storage, const Color& colour, float light_intensity, Vector3 light_direction, bool geometry_mode, bool use_colour_override)
    {
        // There are no vertices.
        if (!_vertex_buffer)
        {
            return;
        }

        auto context = _device->context();

        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        memset(&mapped_resource, 0, sizeof(mapped_resource));

        MeshData data{ world_view_projection, colour, Vector4(light_direction.x, light_direction.y, light_direction.z, 1), light_intensity, light_direction != Vector3::Zero, use_colour_override };
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
                auto texture = geometry_mode ? texture_storage.geometry_texture() : texture_storage.texture(i);
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

    void Mesh::render(const Matrix& world_view_projection, const graphics::Texture& replacement_texture, const DirectX::SimpleMath::Color& colour, float light_intensity, Vector3 light_direction)
    {
        // There are no vertices.
        if (!_vertex_buffer)
        {
            return;
        }

        auto context = _device->context();

        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        memset(&mapped_resource, 0, sizeof(mapped_resource));

        MeshData data{ world_view_projection, colour, Vector4(light_direction.x, light_direction.y, light_direction.z, 1), light_intensity, light_direction != Vector3::Zero };
        context->Map(_matrix_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
        memcpy(mapped_resource.pData, &data, sizeof(data));
        context->Unmap(_matrix_buffer.Get(), 0);

        UINT stride = sizeof(MeshVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, _vertex_buffer.GetAddressOf(), &stride, &offset);
        context->VSSetConstantBuffers(0, 1, _matrix_buffer.GetAddressOf());

        if (_untextured_index_count)
        {
            context->PSSetShaderResources(0, 1, replacement_texture.view().GetAddressOf());
            context->IASetIndexBuffer(_untextured_index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
            context->DrawIndexed(_untextured_index_count, 0, 0);
        }
    }

    std::vector<TransparentTriangle> Mesh::transparent_triangles() const
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
                Intersects(position, direction, tri.v0, tri.v1, tri.v2, distance) &&
                distance < result.distance)
            {
                result.hit = true;
                result.distance = std::min(distance, result.distance);
                result.triangle = tri;
            }
        }

        // Calculate the world space hit position, if there was a hit.
        if (result.hit)
        {
            result.position = position + direction * result.distance;
        }

        return result;
    }
}
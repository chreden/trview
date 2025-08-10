#include "Mesh.h"
#include <ranges>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace
    {
        void reverse(auto&& trio)
        {
            std::swap(trio[0], trio[2]);
        }

        UniTriangle reverse(const UniTriangle& tri)
        {
            UniTriangle copy = tri;
            reverse(copy.vertices);
            reverse(copy.colours);
            for (auto& f : copy.frames)
            {
                reverse(f.uvs);
            }
            return copy;
        }

        void add_tri(auto&& t, auto&& vertices, auto&& indices, auto&& untextured)
        {
            const uint32_t base = static_cast<uint32_t>(vertices.size());
            vertices.push_back(MeshVertex{ .pos = t.vertices[0], .normal = t.normals[0], .uv = t.frames.empty() ? Vector2() : t.frames[0].uvs[0], .colour = t.colours[0] });
            vertices.push_back(MeshVertex{ .pos = t.vertices[1], .normal = t.normals[1], .uv = t.frames.empty() ? Vector2() : t.frames[0].uvs[1], .colour = t.colours[1] });
            vertices.push_back(MeshVertex{ .pos = t.vertices[2], .normal = t.normals[2], .uv = t.frames.empty() ? Vector2() : t.frames[0].uvs[2], .colour = t.colours[2] });
            auto& target_indices = t.texture_mode == UniTriangle::TextureMode::Textured ? indices[t.frames[0].texture] : untextured;
            target_indices.push_back(base);
            target_indices.push_back(base + 1);
            target_indices.push_back(base + 2);
        }
    }

    Mesh::Mesh(const std::shared_ptr<graphics::IDevice>& device, const std::vector<UniTriangle>& triangles, const std::shared_ptr<ITextureStorage>& texture_storage)
        : _device(device), _texture_storage(texture_storage)
    {
        if (!triangles.empty())
        {
            std::vector<MeshVertex> out_vertices;
            std::unordered_map<uint32_t, std::vector<uint32_t>> mapped_indices;
            std::vector<uint32_t> new_untextured_indices;

            // Make everything based off the triangles.
            for (const auto& t : triangles)
            {
                if (t.animation_mode == UniTriangle::AnimationMode::None)
                {
                    if (t.transparency_mode == UniTriangle::TransparencyMode::None)
                    {
                        add_tri(t, out_vertices, mapped_indices, new_untextured_indices);
                        if (t.side_mode == UniTriangle::SideMode::Double)
                        {
                            add_tri(reverse(t), out_vertices, mapped_indices, new_untextured_indices);
                        }
                    }
                    else
                    {
                        _transparent_triangles.push_back(t);
                        if (t.side_mode == UniTriangle::SideMode::Double)
                        {
                            _transparent_triangles.push_back(reverse(t));
                        }
                    }
                }
                else
                {
                    _animated_triangles.push_back(t);
                    if (t.side_mode == UniTriangle::SideMode::Double)
                    {
                        _animated_triangles.push_back(reverse(t));
                    }

                    for (const auto& frame : t.frames)
                    {
                        _animated_triangle_textures.insert(frame.texture);
                    }
                }

                if (t.collision_mode == UniTriangle::CollisionMode::Enabled)
                {
                    _collision_triangles.push_back(t);
                    if (t.side_mode == UniTriangle::SideMode::Double)
                    {
                        _collision_triangles.push_back(reverse(t));
                    }
                }
            }

            if (out_vertices.size())
            {
                // Generate vertex buffer
                D3D11_BUFFER_DESC vertex_desc;
                memset(&vertex_desc, 0, sizeof(vertex_desc));
                vertex_desc.Usage = D3D11_USAGE_DEFAULT;
                vertex_desc.ByteWidth = sizeof(MeshVertex) * static_cast<uint32_t>(out_vertices.size());
                vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
                D3D11_SUBRESOURCE_DATA vertex_data;
                memset(&vertex_data, 0, sizeof(vertex_data));
                vertex_data.pSysMem = &out_vertices[0];
                _vertex_buffer = device->create_buffer(vertex_desc, vertex_data);
            }

            // Generate index buffers
            for (const auto& map_indices : mapped_indices)
            {
                D3D11_BUFFER_DESC index_desc;
                memset(&index_desc, 0, sizeof(index_desc));
                index_desc.Usage = D3D11_USAGE_DEFAULT;
                index_desc.ByteWidth = sizeof(uint32_t) * static_cast<uint32_t>(map_indices.second.size());
                index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

                D3D11_SUBRESOURCE_DATA index_data;
                memset(&index_data, 0, sizeof(index_data));
                index_data.pSysMem = &map_indices.second[0];

                _index_buffers[map_indices.first] =
                {
                    .count = static_cast<uint32_t>(map_indices.second.size()),
                    .buffer = device->create_buffer(index_desc, index_data)
                };
            }

            if (!new_untextured_indices.empty())
            {
                // Untextured indices:
                D3D11_BUFFER_DESC index_desc;
                memset(&index_desc, 0, sizeof(index_desc));
                index_desc.Usage = D3D11_USAGE_DEFAULT;
                index_desc.ByteWidth = sizeof(uint32_t) * static_cast<uint32_t>(new_untextured_indices.size());
                index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

                D3D11_SUBRESOURCE_DATA index_data;
                memset(&index_data, 0, sizeof(index_data));
                index_data.pSysMem = &new_untextured_indices[0];

                _untextured_index_buffer = device->create_buffer(index_desc, index_data);
                _untextured_index_count = static_cast<uint32_t>(new_untextured_indices.size());
            }
        }

        generate_matrix_buffer();
        generate_animated_vertex_buffer();
        calculate_bounding_box(triangles);
    }

    void Mesh::calculate_bounding_box(const std::vector<UniTriangle>& triangles)
    {
        Vector3 minimum(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector3 maximum(-FLT_MAX, -FLT_MAX, -FLT_MAX);
        for (const auto& t : triangles)
        {
            for (const auto& v : t.vertices)
            {
                minimum = Vector3::Min(minimum, v);
                maximum = Vector3::Max(maximum, v);
            }
        }

        const bool no_vertices = triangles.empty();
        const Vector3 half_size = no_vertices ? Vector3::Zero : (maximum - minimum) * 0.5f;
        minimum = no_vertices ? Vector3::Zero : minimum;

        _bounding_box.Extents = half_size;
        _bounding_box.Center = minimum + half_size;
    }

    void Mesh::render(const Matrix& world_view_projection, const Color& colour, float light_intensity, Vector3 light_direction, bool geometry_mode, bool use_colour_override)
    {
        const auto texture_storage = _texture_storage.lock();
        if (!texture_storage)
        {
            return;
        }

        auto context = _device->context();

        if (_vertex_buffer)
        {
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

            if (!_index_buffers.empty())
            {
                for (const auto& indices : _index_buffers)
                {
                    auto texture = geometry_mode ? texture_storage->geometry_texture() : texture_storage->texture(indices.first);
                    context->PSSetShaderResources(0, 1, texture.view().GetAddressOf());
                    context->IASetIndexBuffer(indices.second.buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                    context->DrawIndexed(indices.second.count, 0, 0);
                }
            }

            if (_untextured_index_count)
            {
                auto texture = texture_storage->untextured();
                context->PSSetShaderResources(0, 1, texture.view().GetAddressOf());
                context->IASetIndexBuffer(_untextured_index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
                context->DrawIndexed(_untextured_index_count, 0, 0);
            }
        }

        if (_animated_vertex_buffer)
        {
            D3D11_MAPPED_SUBRESOURCE mapped_resource;
            memset(&mapped_resource, 0, sizeof(mapped_resource));

            MeshData data{ world_view_projection, colour, Vector4(light_direction.x, light_direction.y, light_direction.z, 1), light_intensity, light_direction != Vector3::Zero, use_colour_override };
            context->Map(_matrix_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
            memcpy(mapped_resource.pData, &data, sizeof(data));
            context->Unmap(_matrix_buffer.Get(), 0);

            if (!_animated_triangles.empty())
            {
                for (const auto& tex : _animated_triangle_textures)
                {
                    D3D11_MAPPED_SUBRESOURCE mapped{};
                    if (S_OK == context->Map(_animated_vertex_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped))
                    {
                        MeshVertex* vertex = reinterpret_cast<MeshVertex*>(mapped.pData);
                        uint32_t triangles_written = 0;
                        for (const auto& triangle : _animated_triangles)
                        {
                            if (triangle.transparency_mode == UniTriangle::TransparencyMode::None &&
                                triangle.frames[triangle.current_frame].texture == tex)
                            {
                                ++triangles_written;
                                *vertex++ = { .pos = triangle.vertices[0], .normal = { 0, 1, 0 }, .uv = triangle.frames[triangle.current_frame].uvs[0], .colour = triangle.colours[0] };
                                *vertex++ = { .pos = triangle.vertices[1], .normal = { 0, 1, 0 }, .uv = triangle.frames[triangle.current_frame].uvs[1], .colour = triangle.colours[1] };
                                *vertex++ = { .pos = triangle.vertices[2], .normal = { 0, 1, 0 }, .uv = triangle.frames[triangle.current_frame].uvs[2], .colour = triangle.colours[2] };
                            }
                        }
                        context->Unmap(_animated_vertex_buffer.Get(), 0);

                        if (triangles_written)
                        {
                            UINT stride = sizeof(MeshVertex);
                            UINT offset = 0;
                            context->IASetVertexBuffers(0, 1, _animated_vertex_buffer.GetAddressOf(), &stride, &offset);
                            context->VSSetConstantBuffers(0, 1, _matrix_buffer.GetAddressOf());
                            auto texture = geometry_mode ? texture_storage->geometry_texture() : texture_storage->texture(tex);
                            context->PSSetShaderResources(0, 1, texture.view().GetAddressOf());
                            context->Draw(triangles_written * 3, 0);
                        }
                    }
                }
            }
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
        auto transparent_triangles = _transparent_triangles;
        if (!_animated_triangles.empty())
        {
            transparent_triangles.append_range(_animated_triangles | std::views::filter([](auto&& t) { return t.transparency_mode != UniTriangle::TransparencyMode::None; }));
        }
        return transparent_triangles
            | std::views::transform([](auto&& t)
                {
                    return TransparentTriangle(t.vertices[0], t.vertices[1], t.vertices[2], 
                        t.frames.empty() ? Vector2() : t.frames[t.current_frame].uvs[0],
                        t.frames.empty() ? Vector2() : t.frames[t.current_frame].uvs[1],
                        t.frames.empty() ? Vector2() : t.frames[t.current_frame].uvs[2],
                        t.frames.empty() ? TransparentTriangle::Untextured : t.frames[t.current_frame].texture,
                        t.transparency_mode, t.colours[0], t.colours[1], t.colours[2]);
                })
            | std::ranges::to<std::vector>();
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
            if (direction.Dot(tri.normal()) < 0 &&
                Intersects(position, direction, tri.vertices[0], tri.vertices[1], tri.vertices[2], distance) &&
                distance < result.distance)
            {
                result.hit = true;
                result.distance = std::min(distance, result.distance);
                result.triangle = Triangle(tri.vertices[0], tri.vertices[1], tri.vertices[2]);
            }
        }

        // Calculate the world space hit position, if there was a hit.
        if (result.hit)
        {
            result.position = position + direction * result.distance;
        }

        return result;
    }

    void Mesh::generate_animated_vertex_buffer()
    {
        if (!_animated_triangles.empty())
        {
            D3D11_BUFFER_DESC animated_vertex_desc;
            memset(&animated_vertex_desc, 0, sizeof(animated_vertex_desc));
            animated_vertex_desc.Usage = D3D11_USAGE_DYNAMIC;
            animated_vertex_desc.ByteWidth = sizeof(MeshVertex) * static_cast<uint32_t>(_animated_triangles.size() * 3);
            animated_vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            animated_vertex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            _animated_vertex_buffer = _device->create_buffer(animated_vertex_desc, std::nullopt);
        }
    }

    void Mesh::generate_matrix_buffer()
    {
        D3D11_BUFFER_DESC matrix_desc;
        memset(&matrix_desc, 0, sizeof(matrix_desc));
        matrix_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        matrix_desc.ByteWidth = sizeof(MeshData);
        matrix_desc.Usage = D3D11_USAGE_DYNAMIC;
        matrix_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        _matrix_buffer = _device->create_buffer(matrix_desc, std::optional<D3D11_SUBRESOURCE_DATA>());
    }

    void Mesh::update(float delta)
    {
        if (_animated_triangles.empty())
        {
            return;
        }

        for (auto& triangle : _animated_triangles)
        {
            triangle.current_time += delta;
            if (triangle.current_time >= triangle.frame_time)
            {
                triangle.current_time -= static_cast<int>(triangle.current_time / triangle.frame_time) * triangle.frame_time;
                triangle.current_frame++;
                if (triangle.current_frame >= triangle.frames.size())
                {
                    triangle.current_frame = 0;
                }
            }
        }
    }
}
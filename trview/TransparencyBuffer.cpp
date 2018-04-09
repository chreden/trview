#include "stdafx.h"
#include "TransparencyBuffer.h"

#include "ICamera.h"
#include "ILevelTextureStorage.h"
#include "MeshVertex.h"

#include <algorithm>

namespace trview
{
    TransparencyBuffer::TransparencyBuffer(CComPtr<ID3D11Device> device)
        : _device(device)
    {
        create_matrix_buffer();
    }

    void TransparencyBuffer::add(const TransparentTriangle& triangle)
    {
        _triangles.push_back(triangle);
    }

    void TransparencyBuffer::sort(const DirectX::SimpleMath::Vector3& eye_position)
    {
        using namespace DirectX::SimpleMath;
        std::sort(_triangles.begin(), _triangles.end(),
            [&eye_position](const auto& l, const auto& r)
        {
            return Vector3::DistanceSquared(eye_position, l.position) > Vector3::DistanceSquared(eye_position, r.position);
        });
        complete();
    }

    void TransparencyBuffer::render(CComPtr<ID3D11DeviceContext> context, const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        if (!_triangles.size())
        {
            return;
        }

        using namespace DirectX::SimpleMath;

        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        memset(&mapped_resource, 0, sizeof(mapped_resource));

        struct Data
        {
            Matrix matrix;
            Color colour;
        };

        Data data{ camera.view_projection(), Color(1,1,1,1) };

        context->Map(_matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
        memcpy(mapped_resource.pData, &data, sizeof(data));
        context->Unmap(_matrix_buffer, 0);

        UINT stride = sizeof(MeshVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &_vertex_buffer.p, &stride, &offset);
        context->VSSetConstantBuffers(0, 1, &_matrix_buffer.p);
        context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

        uint32_t sum = 0;
        for (const auto& run : _texture_run)
        {
            auto texture = texture_storage.texture(run.texture);
            context->PSSetShaderResources(0, 1, &texture.view.p);
            context->Draw(run.count * 3, sum);
            sum += run.count * 3;
        }
    }

    void TransparencyBuffer::reset()
    {
        _triangles.clear();
    }

    void TransparencyBuffer::create_buffer()
    {
        _vertex_buffer = nullptr;

        // Generate the buffers.
        D3D11_BUFFER_DESC vertex_desc;
        memset(&vertex_desc, 0, sizeof(vertex_desc));
        vertex_desc.Usage = D3D11_USAGE_DEFAULT;
        vertex_desc.ByteWidth = sizeof(MeshVertex) * _vertices.size();
        vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertex_data;
        memset(&vertex_data, 0, sizeof(vertex_data));
        vertex_data.pSysMem = &_vertices[0];

        _device->CreateBuffer(&vertex_desc, &vertex_data, &_vertex_buffer);
    }

    void TransparencyBuffer::create_matrix_buffer()
    {
        using namespace DirectX::SimpleMath;

        _matrix_buffer = nullptr;

        D3D11_BUFFER_DESC matrix_desc;
        memset(&matrix_desc, 0, sizeof(matrix_desc));

        matrix_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        matrix_desc.ByteWidth = sizeof(Matrix) + sizeof(Color);
        matrix_desc.Usage = D3D11_USAGE_DYNAMIC;
        matrix_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        _device->CreateBuffer(&matrix_desc, nullptr, &_matrix_buffer);
    }

    void TransparencyBuffer::complete()
    {
        // Convert the triangles into mesh vertexes.
        // Also will have to capture the runs of textures.
        _vertices.resize(_triangles.size() * 3);

        _texture_run.clear();

        std::size_t index = 0;
        for (const auto& triangle : _triangles)
        {
            if (_texture_run.empty() || _texture_run.back().texture != triangle.texture)
            {
                _texture_run.push_back({ triangle.texture, 1 });
            }
            else
            {
                ++_texture_run.back().count;
            }

            for (uint32_t i = 0; i < 3; ++i)
            {
                _vertices[index++] = { triangle.vertices[i], triangle.uvs[i], triangle.colour };
            }
        }

        create_buffer();
    }
}

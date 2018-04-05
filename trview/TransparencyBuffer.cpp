#include "stdafx.h"
#include "TransparencyBuffer.h"

#include "ICamera.h"
#include "ILevelTextureStorage.h"
#include "MeshVertex.h"

#include <algorithm>

namespace trview
{
    TransparentTriangle TransparentTriangle::transform(const DirectX::SimpleMath::Matrix& matrix, const DirectX::SimpleMath::Color& colour) const
    {
        using namespace DirectX::SimpleMath;
        TransparentTriangle result(
            Vector3::Transform(vertices[0], matrix),
            Vector3::Transform(vertices[1], matrix),
            Vector3::Transform(vertices[2], matrix),
            uvs[0], uvs[1], uvs[2], texture);
        Vector3 minimum = Vector3::Min(Vector3::Min(result.vertices[0], result.vertices[1]), result.vertices[2]);
        Vector3 maximum = Vector3::Max(Vector3::Max(result.vertices[0], result.vertices[1]), result.vertices[2]);
        result.position = Vector3::Lerp(minimum, maximum, 0.5f);
        result.colour = colour;
        return result;
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
    }

    void TransparencyBuffer::render(CComPtr<ID3D11Device> device, CComPtr<ID3D11DeviceContext> context, const ICamera& camera, const ILevelTextureStorage& texture_storage)
    {
        if (!_triangles.size())
        {
            return;
        }

        // Convert the triangles into mesh vertexes.
        // Also will have to capture the runs of textures.
        _vertices.resize(_triangles.size() * 3);

        struct TextureRun
        {
            uint32_t texture;
            uint32_t count;
        };

        std::vector<TextureRun> texture_run;

        std::size_t index = 0;
        for (const auto& triangle : _triangles)
        {
            if (texture_run.empty() || texture_run.back().texture != triangle.texture)
            {
                texture_run.push_back({ triangle.texture, 1 });
            }
            else
            {
                ++texture_run.back().count;
            }

            for (uint32_t i = 0; i < 3; ++i)
            {
                _vertices[index++]= { triangle.vertices[i], triangle.uvs[i], triangle.colour };
            }
        }

        // Generate the buffers.
        D3D11_BUFFER_DESC vertex_desc;
        memset(&vertex_desc, 0, sizeof(vertex_desc));
        vertex_desc.Usage = D3D11_USAGE_DEFAULT;
        vertex_desc.ByteWidth = sizeof(MeshVertex) * _vertices.size();
        vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertex_data;
        memset(&vertex_data, 0, sizeof(vertex_data));
        vertex_data.pSysMem = &_vertices[0];

        CComPtr<ID3D11Buffer> vertex_buffer;
        HRESULT hr = device->CreateBuffer(&vertex_desc, &vertex_data, &vertex_buffer);

        using namespace DirectX::SimpleMath;

        D3D11_BUFFER_DESC matrix_desc;
        memset(&matrix_desc, 0, sizeof(matrix_desc));

        matrix_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        matrix_desc.ByteWidth = sizeof(Matrix) + sizeof(Color);
        matrix_desc.Usage = D3D11_USAGE_DYNAMIC;
        matrix_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        CComPtr<ID3D11Buffer> matrix_buffer;
        device->CreateBuffer(&matrix_desc, nullptr, &matrix_buffer);

        // Render the triangles in order (changing the texture where appropriate).
        // Actual rendering here...

        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        memset(&mapped_resource, 0, sizeof(mapped_resource));

        struct Data
        {
            Matrix matrix;
            Color colour;
        };

        Data data{ camera.view_projection(), Color(1,1,1,1) };

        context->Map(matrix_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
        memcpy(mapped_resource.pData, &data, sizeof(data));
        context->Unmap(matrix_buffer, 0);

        UINT stride = sizeof(MeshVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &vertex_buffer.p, &stride, &offset);
        context->VSSetConstantBuffers(0, 1, &matrix_buffer.p);

        context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

        uint32_t sum = 0;
        for (const auto& run : texture_run)
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
}

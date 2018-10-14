#include "stdafx.h"
#include "TransparencyBuffer.h"

#include "ICamera.h"
#include <trview.app/ILevelTextureStorage.h>
#include <trview.app/MeshVertex.h>

#include <algorithm>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    TransparencyBuffer::TransparencyBuffer(const ComPtr<ID3D11Device>& device)
        : _device(device)
    {
        create_matrix_buffer();

        D3D11_BLEND_DESC alpha_desc;
        memset(&alpha_desc, 0, sizeof(alpha_desc));
        alpha_desc.RenderTarget[0].BlendEnable = true;
        alpha_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        alpha_desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        alpha_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        alpha_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        alpha_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        alpha_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        alpha_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        _device->CreateBlendState(&alpha_desc, &_alpha_blend);

        D3D11_BLEND_DESC additive_desc;
        memset(&additive_desc, 0, sizeof(additive_desc));
        additive_desc.RenderTarget[0].BlendEnable = true;
        additive_desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        additive_desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
        additive_desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        additive_desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        additive_desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
        additive_desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        additive_desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        _device->CreateBlendState(&additive_desc, &_additive_blend);

        // Depth stencil that will test depth, but will not write it.
        D3D11_DEPTH_STENCIL_DESC stencil_desc;
        memset(&stencil_desc, 0, sizeof(stencil_desc));
        stencil_desc.DepthEnable = true;
        stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;
        stencil_desc.StencilEnable = true;
        stencil_desc.StencilReadMask = 0xFF;
        stencil_desc.StencilWriteMask = 0xFF;
        stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        _device->CreateDepthStencilState(&stencil_desc, &_transparency_depth_state);
    }

    void TransparencyBuffer::add(const TransparentTriangle& triangle)
    {
        _triangles.push_back(triangle);
    }

    void TransparencyBuffer::sort(const Vector3& eye_position)
    {
        std::sort(_triangles.begin(), _triangles.end(),
            [&eye_position](const auto& l, const auto& r)
        {
            return Vector3::DistanceSquared(eye_position, l.position) > Vector3::DistanceSquared(eye_position, r.position);
        });
        complete();
    }

    void TransparencyBuffer::render(const ComPtr<ID3D11DeviceContext>& context, const ICamera& camera, const ILevelTextureStorage& texture_storage, bool ignore_blend)
    {
        if (!_vertices.size())
        {
            return;
        }

        ComPtr<ID3D11DepthStencilState> old_depth_state;
        ComPtr<ID3D11BlendState> old_blend_state;
        context->OMGetDepthStencilState(&old_depth_state, nullptr);
        context->OMGetBlendState(&old_blend_state, nullptr, nullptr);

        context->OMSetDepthStencilState(_transparency_depth_state.Get(), 1);

        D3D11_MAPPED_SUBRESOURCE mapped_resource;
        memset(&mapped_resource, 0, sizeof(mapped_resource));

        struct Data
        {
            Matrix matrix;
            Color colour;
        };

        Data data{ camera.view_projection(), Color(1,1,1,1) };

        context->Map(_matrix_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
        memcpy(mapped_resource.pData, &data, sizeof(data));
        context->Unmap(_matrix_buffer.Get(), 0);

        UINT stride = sizeof(MeshVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, _vertex_buffer.GetAddressOf(), &stride, &offset);
        context->VSSetConstantBuffers(0, 1, _matrix_buffer.GetAddressOf());
        context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
        context->OMSetBlendState(_alpha_blend.Get(), 0, 0xffffffff);

        uint32_t sum = 0;
        TransparentTriangle::Mode previous_mode = TransparentTriangle::Mode::Normal;

        // Untextured texture for transparent triangles that don't use the texture storage indexes.
        auto untextured = texture_storage.coloured(0xffffffff);

        for (const auto& run : _texture_run)
        {
            if (run.mode != previous_mode && !ignore_blend)
            {
                set_blend_mode(context, run.mode);
            }
            previous_mode = run.mode;

            auto texture = run.texture == TransparentTriangle::Untextured ? untextured : texture_storage.texture(run.texture);
            context->PSSetShaderResources(0, 1, texture.view().GetAddressOf());
            context->Draw(run.count * 3, sum);
            sum += run.count * 3;
        }

        context->OMSetDepthStencilState(old_depth_state.Get(), 1);
        context->OMSetBlendState(old_blend_state.Get(), nullptr, 0xffffffff);
    }

    void TransparencyBuffer::reset()
    {
        _triangles.clear();
    }

    void TransparencyBuffer::create_buffer()
    {
        _vertex_buffer = nullptr;

        if (_vertices.empty())
        {
            return;
        }

        // Generate the buffers.
        D3D11_BUFFER_DESC vertex_desc;
        memset(&vertex_desc, 0, sizeof(vertex_desc));
        vertex_desc.Usage = D3D11_USAGE_DEFAULT;
        vertex_desc.ByteWidth = sizeof(MeshVertex) * static_cast<uint32_t>(_vertices.size());
        vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertex_data;
        memset(&vertex_data, 0, sizeof(vertex_data));
        vertex_data.pSysMem = &_vertices[0];

        _device->CreateBuffer(&vertex_desc, &vertex_data, &_vertex_buffer);
    }

    void TransparencyBuffer::create_matrix_buffer()
    {
        _matrix_buffer = nullptr;

        D3D11_BUFFER_DESC matrix_desc;
        memset(&matrix_desc, 0, sizeof(matrix_desc));

        matrix_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        matrix_desc.ByteWidth = sizeof(Matrix) + sizeof(Color);
        matrix_desc.Usage = D3D11_USAGE_DYNAMIC;
        matrix_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        _device->CreateBuffer(&matrix_desc, nullptr, _matrix_buffer.GetAddressOf());
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
            if (_texture_run.empty() ||
                _texture_run.back().texture != triangle.texture || 
                _texture_run.back().mode != triangle.mode) 
            {
                _texture_run.push_back({ triangle.texture, triangle.mode, 1 });
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

    void TransparencyBuffer::set_blend_mode(const ComPtr<ID3D11DeviceContext>& context, TransparentTriangle::Mode mode) const
    {
        if (mode == TransparentTriangle::Mode::Normal)
        {
            context->OMSetBlendState(_alpha_blend.Get(), 0, 0xffffffff);
        }
        else
        {
            context->OMSetBlendState(_additive_blend.Get(), 0, 0xffffffff);
        }
    }
}

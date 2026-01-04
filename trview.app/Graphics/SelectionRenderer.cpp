#include "SelectionRenderer.h"
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IShader.h>
#include <trview.graphics/IRenderTarget.h>
#include <trview.graphics/RenderTargetStore.h>
#include <trview.graphics/VertexShaderStore.h>
#include <trview.graphics/PixelShaderStore.h>
#include <trview.app/Elements/Trigger.h>

#include <trview.app/Geometry/IRenderable.h>
#include <trview.app/Camera/ICamera.h>

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    using namespace graphics;

    namespace
    {
        struct SelectionVertex
        {
            Vector3 pos;
            Vector2 uv;
        };

        __declspec(align(16))
        struct PS_Data
        {
            float pixel_width;
            float pixel_height;
            double _; // Padding.
            Color outline_colour;
        }; 
    }

    ISelectionRenderer::~ISelectionRenderer()
    {
    }

    SelectionRenderer::SelectionRenderer(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<graphics::IShaderStorage>& shader_storage, std::unique_ptr<ITransparencyBuffer> transparency, const graphics::IRenderTarget::SizeSource& render_target_source)
        : _device(device), _transparency(std::move(transparency)), _render_target_source(render_target_source)
    {
        _pixel_shader = shader_storage->get("selection_pixel_shader");
        _vertex_shader = shader_storage->get("ui_vertex_shader");

        D3D11_SAMPLER_DESC sampler_desc;
        memset(&sampler_desc, 0, sizeof(sampler_desc));
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        sampler_desc.MaxAnisotropy = 1;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
        _sampler_state = device->create_sampler_state(sampler_desc);

        create_buffers(*device);
    }

    void SelectionRenderer::create_buffers(const graphics::IDevice& device)
    {
        const SelectionVertex vertices[] =
        {
            { Vector3(-1.0f, 1.0f, 0.0f), Vector2::Zero },
            { Vector3(1.0f, 1.0f, 0.0f), Vector2(1,0) },
            { Vector3(-1.0f, -1.0f, 0.0f), Vector2(0,1) },
            { Vector3(1.0f, -1.0f, 0.0f), Vector2(1,1) }
        };

        D3D11_BUFFER_DESC vertex_desc;
        memset(&vertex_desc, 0, sizeof(vertex_desc));
        vertex_desc.Usage = D3D11_USAGE_DEFAULT;
        vertex_desc.ByteWidth = sizeof(SelectionVertex) * 4;
        vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertex_data;
        memset(&vertex_data, 0, sizeof(vertex_data));
        vertex_data.pSysMem = vertices;

        _vertex_buffer = device.create_buffer(vertex_desc, vertex_data);

        uint32_t indices[] = { 0, 1, 2, 3 };

        D3D11_BUFFER_DESC index_desc;
        memset(&index_desc, 0, sizeof(index_desc));
        index_desc.Usage = D3D11_USAGE_DEFAULT;
        index_desc.ByteWidth = sizeof(uint32_t) * 4;
        index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA index_data;
        memset(&index_data, 0, sizeof(index_data));
        index_data.pSysMem = indices;

        _index_buffer = device.create_buffer(index_desc, index_data);

        using namespace DirectX::SimpleMath;
        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(desc));

        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = sizeof(MeshData);
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        _matrix_buffer = device.create_buffer(desc, std::optional<D3D11_SUBRESOURCE_DATA>());

        using namespace DirectX::SimpleMath;
        D3D11_BUFFER_DESC scale_desc;
        memset(&scale_desc, 0, sizeof(scale_desc));

        scale_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        scale_desc.ByteWidth = sizeof(PS_Data);
        scale_desc.Usage = D3D11_USAGE_DYNAMIC;
        scale_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        _scale_buffer = device.create_buffer(scale_desc, std::optional<D3D11_SUBRESOURCE_DATA>());
    }

    void SelectionRenderer::render(const ICamera& camera, IRenderable& selected_item, const DirectX::SimpleMath::Color& outline_colour)
    {
        auto context = _device->context();

        // Get viewport size - this is used for checking if the texture size needs to change and for
        // scaling the coordinates that the pixel shader uses for the edge detection.
        uint32_t num_viewports = 1;
        D3D11_VIEWPORT viewport;
        context->RSGetViewports(&num_viewports, &viewport);

        // If the texture hasn't been made yet or the size needs to change, re-create the texture.
        if (!_texture || _texture->size() != Size(viewport.Width, viewport.Height))
        {
            _texture = _render_target_source(static_cast<uint32_t>(viewport.Width), static_cast<uint32_t>(viewport.Height), IRenderTarget::DepthStencilMode::Enabled);
        }

        // Render the item (all regular faces and transparent faces) to a render target.
        {
            // Clear the render target with red. This also clears depth. Start rendering to the render target.
            graphics::RenderTargetStore store(context);
            _texture->clear(Color(1.0f, 0.0f, 0.0f, 1.0f));
            _texture->apply();

            // Draw the regular faces of the item with a black colouring.
            const bool was_visible = selected_item.visible();
            selected_item.set_visible(true);
            selected_item.render(camera, IRenderable::SelectionFill);

            // Also render the transparent parts of the meshes, again with black.
            _transparency->reset();
            selected_item.get_transparent_triangles(*_transparency, camera, IRenderable::SelectionFill);
            _transparency->sort(camera.rendering_position());
            _transparency->render(camera.view_projection(), true);
            selected_item.set_visible(was_visible);
        }

        // Set vertex shader parameters. Since we don't require any kind of scaling (we just want to fill the viewport), 
        // identity matrix is fine for scale.
        {
            D3D11_MAPPED_SUBRESOURCE mapped_resource;
            memset(&mapped_resource, 0, sizeof(mapped_resource));

            MeshData data{ Matrix::Identity, Color(1,1,1,1), Vector4::Zero };
            context->Map(_matrix_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
            memcpy(mapped_resource.pData, &data, sizeof(data));
            context->Unmap(_matrix_buffer.Get(), 0);
        }

        // Set the pixel shader parameters. It only needs to know about the width and height of the viewport, so it knows
        // what coordinates to use to the get the next pixel over.
        {
            D3D11_MAPPED_SUBRESOURCE mapped_resource;
            memset(&mapped_resource, 0, sizeof(mapped_resource));

            PS_Data data{ 1.0f / viewport.Width, 1.0f / viewport.Height, 0, outline_colour };
            context->Map(_scale_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
            memcpy(mapped_resource.pData, &data, sizeof(data));
            context->Unmap(_scale_buffer.Get(), 0);
        }

        // Render the texture to the screen using the pixel shader and vertex shader. The vertex shader doesn't do anything
        // special, but the pixel shader will find where the red stops and make a white border and remove the black parts of
        // the image (so the actual item can be seen underneath).
        VertexShaderStore vs_store(context);
        PixelShaderStore ps_store(context);

        _vertex_shader->apply(context);
        _pixel_shader->apply(context);
        
        context->PSSetSamplers(0, 1, _sampler_state.GetAddressOf());
        context->PSSetShaderResources(0, 1, _texture->texture().view().GetAddressOf());
        UINT stride = sizeof(SelectionVertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, _vertex_buffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(_index_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        context->VSSetConstantBuffers(0, 1, _matrix_buffer.GetAddressOf());
        context->PSSetConstantBuffers(0, 1, _scale_buffer.GetAddressOf());
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
        context->DrawIndexed(4, 0, 0);

        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        constexpr std::array<ID3D11ShaderResourceView*, 1> null{ nullptr };
        context->PSSetShaderResources(0, 1, &null[0]);
    }
}

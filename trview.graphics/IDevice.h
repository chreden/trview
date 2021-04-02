#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <optional>
#include <trview.common/Window.h>

namespace trview
{
    namespace graphics
    {
        class DeviceWindow;

        struct IDevice
        {
            virtual ~IDevice() = 0;

            /// Begin rendering.
            virtual void begin() = 0;

            /// Gets the D3D device interface.
            /// @returns The D3D device interface.
            virtual Microsoft::WRL::ComPtr<ID3D11Device> device() const = 0;

            /// Gets the D3D device context for the device.
            /// @returns The D3D device context.
            virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext> context() const = 0;

            virtual Microsoft::WRL::ComPtr<ID3D11BlendState> create_blend_state(const D3D11_BLEND_DESC& blend_desc) const = 0;
            virtual Microsoft::WRL::ComPtr<ID3D11Buffer> create_buffer(const D3D11_BUFFER_DESC& buffer_desc, const std::optional<D3D11_SUBRESOURCE_DATA>& buffer_data) const = 0;
            virtual Microsoft::WRL::ComPtr<ID3D11DepthStencilState> create_depth_stencil_state(const D3D11_DEPTH_STENCIL_DESC& depth_stencil_desc) const = 0;
            virtual Microsoft::WRL::ComPtr<ID3D11DepthStencilView> create_depth_stencil_view(const Microsoft::WRL::ComPtr<ID3D11Resource>& resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& depth_stencil_view_desc) const = 0;
            virtual Microsoft::WRL::ComPtr<ID3D11RasterizerState> create_rasterizer_state(const D3D11_RASTERIZER_DESC& rasterizer_desc) const = 0;
            virtual Microsoft::WRL::ComPtr<ID3D11RenderTargetView> create_render_target_view(const Microsoft::WRL::ComPtr<ID3D11Resource>& resource) const = 0;
            virtual Microsoft::WRL::ComPtr<ID3D11SamplerState> create_sampler_state(const D3D11_SAMPLER_DESC& sampler_desc) const = 0;
            virtual Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> create_shader_resource_view(const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture) const = 0;
            virtual Microsoft::WRL::ComPtr<ID3D11Texture2D> create_texture_2D(const D3D11_TEXTURE2D_DESC& texture_desc, const D3D11_SUBRESOURCE_DATA& texture_data) const = 0;
        };
    }
}

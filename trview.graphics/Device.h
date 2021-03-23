/// @file Device.h
/// @brief Wraps the D3D device.
/// 
/// Wraps the D3D device and manages common D3D operations.

#pragma once

#include "IDevice.h"

namespace trview
{
    namespace graphics
    {
        class RenderTarget;

        /// Wraps the D3D device and manages common D3D operations.
        class Device final : public IDevice
        {
        public:
            /// Create a new device.
            Device();

            /// Destructor for the Device class.
            ~Device();

            /// Begin rendering.
            virtual void begin() override;

            /// Gets the D3D device interface.
            /// @returns The D3D device interface.
            virtual Microsoft::WRL::ComPtr<ID3D11Device> device() const override;

            /// Gets the D3D device context for the device.
            /// @returns The D3D device context.
            virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext> context() const override;

            /// Create a device window to render to a specific window.
            /// @param window The window to render to.
            /// @returns The device window object.
            virtual std::unique_ptr<DeviceWindow> create_for_window(const Window& window) override;
            virtual Microsoft::WRL::ComPtr<ID3D11BlendState> create_blend_state(const D3D11_BLEND_DESC& blend_desc) const override;
            virtual Microsoft::WRL::ComPtr<ID3D11Buffer> create_buffer(const D3D11_BUFFER_DESC& buffer_desc, const std::optional<D3D11_SUBRESOURCE_DATA>& buffer_data) const override;
            virtual Microsoft::WRL::ComPtr<ID3D11DepthStencilState> create_depth_stencil_state(const D3D11_DEPTH_STENCIL_DESC& depth_stencil_desc) const override;
            virtual Microsoft::WRL::ComPtr<ID3D11DepthStencilView> create_depth_stencil_view(const Microsoft::WRL::ComPtr<ID3D11Resource>& resource, const D3D11_DEPTH_STENCIL_VIEW_DESC& depth_stencil_view_desc) const override;
            virtual Microsoft::WRL::ComPtr<ID3D11RasterizerState> create_rasterizer_state(const D3D11_RASTERIZER_DESC& rasterizer_desc) const override;
            virtual Microsoft::WRL::ComPtr<ID3D11RenderTargetView> create_render_target_view(const Microsoft::WRL::ComPtr<ID3D11Resource>& resource) const override;
            virtual Microsoft::WRL::ComPtr<ID3D11SamplerState> create_sampler_state(const D3D11_SAMPLER_DESC& sampler_desc) const override;
            virtual Microsoft::WRL::ComPtr<ID3D11Texture2D> create_texture_2D(const D3D11_TEXTURE2D_DESC& texture_desc, const D3D11_SUBRESOURCE_DATA& texture_data) const override;
            virtual Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> create_shader_resource_view(const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture) const override;
        private:
            Microsoft::WRL::ComPtr<ID3D11Device>        _device;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
            Microsoft::WRL::ComPtr<ID3D11BlendState>    _blend_state;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depth_stencil_state;
        };
    }
}

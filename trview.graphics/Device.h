/// @file Device.h
/// @brief Wraps the D3D device.
/// 
/// Wraps the D3D device and manages common D3D operations.

#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <SimpleMath.h>

#include <trview.common/Window.h>

namespace trview
{
    namespace graphics
    {
        class RenderTarget;

        /// Wraps the D3D device and manages common D3D operations.
        class Device final
        {
        public:
            /// Create a new device to render to the specified window.
            /// @param window The window to render to.
            Device(const Window& window);

            /// Destructor for the Device class.
            ~Device();

            /// Begin rendering to the main render target.
            void begin();

            /// Clear the render target with the specified colour.
            /// @param colour The colour to with which to clear the render target.
            void clear(const DirectX::SimpleMath::Color& colour);

            /// Gets the D3D device interface.
            /// @returns The D3D device interface.
            const Microsoft::WRL::ComPtr<ID3D11Device>& device() const;

            /// Gets the D3D device context for the device.
            /// @returns The D3D device context.
            const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context() const;

            /// Presents the rendered output to the swap chain.
            void present();

            /// Resizes the device and any associated resources.
            void resize();
        private:
            /// Create the render target based on the current swap chain.
            void create_render_target();

            Microsoft::WRL::ComPtr<IDXGISwapChain>      _swap_chain;
            Microsoft::WRL::ComPtr<ID3D11Device>        _device;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
            Microsoft::WRL::ComPtr<ID3D11BlendState>    _blend_state;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depth_stencil_state;
            std::unique_ptr<graphics::RenderTarget>     _render_target;
        };
    }
}

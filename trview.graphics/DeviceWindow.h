/// @file DeviceWindow.h
/// @brief Wraps the D3D resources for rendering to a window.

#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <trview.common/Window.h>
#include <trview.common/Colour.h>
#include "RenderTarget.h"

namespace trview
{
    namespace graphics
    {
        class Device;

        /// Wraps the D3D resources for rendering to a window.
        class DeviceWindow final
        {
        public:
            /// Create a new device window.
            /// @param device The device to use to render.
            /// @param window The window to render to.
            explicit DeviceWindow(const Device& device, const Window& window);

            /// Begin rendering to the main render target.
            void begin();

            /// Clear the render target with the specified colour.
            /// @param colour The colour to with which to clear the render target.
            void clear(const DirectX::SimpleMath::Color& colour);

            /// Presents the rendered output to the swap chain.
            /// @param vsync Whether vsync is enabled.
            void present(bool vsync);

            /// Resizes the device and any associated resources.
            void resize();
        private:
            void create_render_target();

            const Device&                               _device;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
            Microsoft::WRL::ComPtr<IDXGISwapChain> _swap_chain;
            std::unique_ptr<RenderTarget> _render_target;
        };
    }
}

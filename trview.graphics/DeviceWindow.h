/// @file DeviceWindow.h
/// @brief Wraps the D3D resources for rendering to a window.

#pragma once

#include <trview.common/Window.h>
#include <trview.common/Colour.h>
#include "IRenderTarget.h"
#include "IDevice.h"
#include "IDeviceWindow.h"

namespace trview
{
    namespace graphics
    {
        /// Wraps the D3D resources for rendering to a window.
        class DeviceWindow final : public IDeviceWindow
        {
        public:
            /// Create a new device window.
            /// @param device The device to use to render.
            /// @param window The window to render to.
            explicit DeviceWindow(const std::shared_ptr<IDevice>& device, const IRenderTarget::TextureSource& render_target_source, const Window& window);

            virtual ~DeviceWindow() = default;

            /// Begin rendering to the main render target.
            virtual void begin() override;

            /// Clear the render target with the specified colour.
            /// @param colour The colour to with which to clear the render target.
            virtual void clear(const DirectX::SimpleMath::Color& colour) override;

            /// Presents the rendered output to the swap chain.
            /// @param vsync Whether vsync is enabled.
            virtual void present(bool vsync) override;

            /// Resizes the device and any associated resources.
            virtual void resize() override;

            virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext> context() const override;
        private:
            void create_render_target();

            std::shared_ptr<IDevice> _device;
            Microsoft::WRL::ComPtr<IDXGISwapChain> _swap_chain;
            std::unique_ptr<IRenderTarget> _render_target;
            IRenderTarget::TextureSource _render_target_source;
        };
    }
}

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
        class DeviceWindow;

        /// Wraps the D3D device and manages common D3D operations.
        class Device final
        {
        public:
            /// Create a new device.
            Device();

            /// Destructor for the Device class.
            ~Device();

            /// Begin rendering.
            void begin();

            /// Gets the D3D device interface.
            /// @returns The D3D device interface.
            const Microsoft::WRL::ComPtr<ID3D11Device>& device() const;

            /// Gets the D3D device context for the device.
            /// @returns The D3D device context.
            const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context() const;

            /// Create a device window to render to a specific window.
            /// @param window The window to render to.
            /// @returns The device window object.
            std::unique_ptr<DeviceWindow> create_for_window(HWND window);
        private:
            Microsoft::WRL::ComPtr<ID3D11Device>        _device;
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
            Microsoft::WRL::ComPtr<ID3D11BlendState>    _blend_state;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilState> _depth_stencil_state;
        };
    }
}

#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
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
            virtual const Microsoft::WRL::ComPtr<ID3D11Device>& device() const = 0;

            /// Gets the D3D device context for the device.
            /// @returns The D3D device context.
            virtual const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context() const = 0;

            /// Create a device window to render to a specific window.
            /// @param window The window to render to.
            /// @returns The device window object.
            virtual std::unique_ptr<DeviceWindow> create_for_window(const Window& window) = 0;
        };
    }
}

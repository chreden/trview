#pragma once

#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <trview.common/Window.h>

namespace trview
{
    namespace graphics
    {
        struct IDeviceWindow
        {
            using Source = std::function<std::unique_ptr<IDeviceWindow>(const Window&)>;

            virtual ~IDeviceWindow() = 0;

            /// Begin rendering to the main render target.
            virtual void begin() = 0;

            /// Clear the render target with the specified colour.
            /// @param colour The colour to with which to clear the render target.
            virtual void clear(const DirectX::SimpleMath::Color& colour) = 0;

            /// Presents the rendered output to the swap chain.
            /// @param vsync Whether vsync is enabled.
            virtual void present(bool vsync) = 0;

            /// Resizes the device and any associated resources.
            virtual void resize() = 0;

            virtual Microsoft::WRL::ComPtr<ID3D11DeviceContext> context() const = 0;
        };
    }
}

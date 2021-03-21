#pragma once

#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>
#include <trview.graphics/Device.h>

#include "Texture.h"

namespace trview
{
    namespace graphics
    {
        // Class to create and manage depth stencil state usage.
        class DepthStencil final
        {
        public:
            // Create a depth stencil of the specified dimensions.
            // device: The device to use to create the depth stencil.
            // width: The width of the depth stencil.
            // height: The height of the depth stencil.
            DepthStencil(const graphics::IDevice& device, uint32_t width, uint32_t height);

            // Get the depth stencil view for the depth stencil.
            // Returns: The depth stencil view.
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> view() const;

            // Clear the depth stencil.
            // context: The device context.
            void clear(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
        private:
            Texture _texture;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> _view;
        };
    }
}


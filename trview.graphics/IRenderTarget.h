#pragma once

#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h>
#include <trview.common/Size.h>
#include "Texture.h"

namespace trview
{
    namespace graphics
    {
        struct IRenderTarget
        {
            // Determines whether the render target should have a depth stencil.
            enum class DepthStencilMode
            {
                // No depth stencil is created.
                Disabled,
                // Depth stencil is created.
                Enabled
            };

            using SizeSource = std::function<std::unique_ptr<IRenderTarget>(uint32_t, uint32_t, DepthStencilMode)>;
            using TextureSource = std::function<std::unique_ptr<IRenderTarget>(const Microsoft::WRL::ComPtr<ID3D11Texture2D>&, DepthStencilMode)>;

            virtual ~IRenderTarget() = 0;

            // Clear the render target.
            // colour: The colour with which to clear the render target.
            virtual void clear(const DirectX::SimpleMath::Color& colour) = 0;

            // Set the render target as the current render target. This will also apply a viewport that matches the 
            // dimensions of the render target.
            virtual void apply() = 0;

            // Get the texture for the render target.
            // Returns: The texture.
            virtual Texture texture() const = 0;

            // Get the render target interface for the render target.
            // Returns: The render target.
            virtual Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target() const = 0;

            // Get the width of the render target in pixels.
            // Returns: The width.
            virtual uint32_t width() const = 0;

            // Get the height of the render target in pixels.
            // Returns: The height.
            virtual uint32_t height() const = 0;

            // Get the size of the render target in pixels.
            // Returns: The size of the render target.
            virtual Size size() const = 0;
        };
    }
}

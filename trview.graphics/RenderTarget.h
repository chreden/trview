#pragma once

#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h>
#include <trview.common/Size.h>
#include <memory>

#include "Texture.h"

namespace trview
{
    namespace graphics
    {
        class DepthStencil;

        // Class to create and manage render target usage.
        class RenderTarget final
        {
        public:
            // Determines whether the render target should have a depth stencil.
            enum class DepthStencilMode
            {
                // No depth stencil is created.
                Disabled,
                // Depth stencil is created.
                Enabled
            };

            // Create a render target of the specified dimensions. The new render target will have its pixels initialised to zero.
            // device: The D3D device.
            // width: The width of the new render target.
            // height: The height of the new render target.
            // depth_mode: Whether a depth stencil should be created.
            RenderTarget(const graphics::IDevice& device, uint32_t width, uint32_t height, DepthStencilMode depth_mode = DepthStencilMode::Disabled);

            // Create a render target using the specfied pre-existing texture.
            // device: The D3D device.
            // texture: The texture to use as the render target.
            // depth_mode: Whether a depth stencil should be created.
            RenderTarget(const graphics::IDevice& device, const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture, DepthStencilMode depth_mode = DepthStencilMode::Disabled);

            ~RenderTarget();

            // Clear the render target.
            // context: The D3D device context.
            // colour: The colour with which to clear the render target.
            void clear(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const DirectX::SimpleMath::Color& colour);

            // Set the render target as the current render target. This will also apply a viewport that matches the 
            // dimensions of the render target.
            // context: The D3D device context.
            void apply(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

            // Get the texture for the render target.
            // Returns: The texture.
            const Texture& texture() const;

            // Get the render target interface for the render target.
            // Returns: The render target.
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target() const;

            // Get the width of the render target in pixels.
            // Returns: The width.
            uint32_t width() const;

            // Get the height of the render target in pixels.
            // Returns: The height.
            uint32_t height() const;

            // Get the size of the render target in pixels.
            // Returns: The size of the render target.
            Size size() const;
        private:
            Texture _texture;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   _view;
            std::unique_ptr<DepthStencil> _depth_stencil;
            uint32_t _width;
            uint32_t _height;
        };
    }
}


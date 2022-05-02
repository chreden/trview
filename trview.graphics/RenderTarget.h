#pragma once

#include <memory>
#include "IRenderTarget.h"
#include "DepthStencil.h"

namespace trview
{
    namespace graphics
    {
        // Class to create and manage render target usage.
        class RenderTarget final : public IRenderTarget
        {
        public:
            // Create a render target of the specified dimensions. The new render target will have its pixels initialised to zero.
            // device: The D3D device.
            // width: The width of the new render target.
            // height: The height of the new render target.
            // depth_mode: Whether a depth stencil should be created.
            RenderTarget(const std::shared_ptr<IDevice>& device, uint32_t width, uint32_t height, DepthStencilMode depth_mode = DepthStencilMode::Disabled);

            // Create a render target using the specfied pre-existing texture.
            // device: The D3D device.
            // texture: The texture to use as the render target.
            // depth_mode: Whether a depth stencil should be created.
            RenderTarget(const std::shared_ptr<IDevice>& device, const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture, DepthStencilMode depth_mode = DepthStencilMode::Disabled);

            virtual ~RenderTarget() = default;

            // Clear the render target.
            // colour: The colour with which to clear the render target.
            virtual void clear(const DirectX::SimpleMath::Color& colour) override;

            // Set the render target as the current render target. This will also apply a viewport that matches the 
            // dimensions of the render target.
            virtual void apply() override;

            // Get the texture for the render target.
            // Returns: The texture.
            virtual Texture texture() const override;

            // Get the render target interface for the render target.
            // Returns: The render target.
            virtual Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target() const override;

            // Get the width of the render target in pixels.
            // Returns: The width.
            virtual uint32_t width() const override;

            // Get the height of the render target in pixels.
            // Returns: The height.
            virtual uint32_t height() const override;

            // Get the size of the render target in pixels.
            // Returns: The size of the render target.
            virtual Size size() const override;
        private:
            std::shared_ptr<IDevice> _device;
            Texture _texture;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _view;
            std::unique_ptr<DepthStencil> _depth_stencil;
            uint32_t _width;
            uint32_t _height;
        };
    }
}


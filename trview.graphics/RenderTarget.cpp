#include "RenderTarget.h"
#include "DepthStencil.h"

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace graphics
    {
        namespace
        {
            ID3D11DepthStencilView* get_depth_stencil(const std::unique_ptr<DepthStencil>& depth_stencil)
            {
                if (depth_stencil)
                {
                    return depth_stencil->view().Get();
                }
                return nullptr;
            }
        }

        // Create a render target of the specified dimensions. The new render target will
        // have its pixels initialised to zero.
        // device: The D3D device.
        // width: The width of the new render target.
        // height: The height of the new render target.
        // depth_mode: Whether a depth stencil should be created.
        RenderTarget::RenderTarget(const graphics::IDevice& device, uint32_t width, uint32_t height, DepthStencilMode depth_mode)
            : _width(width), _height(height), 
            _texture(device, width, height, Texture::Bind::RenderTarget)
        {
            if (depth_mode == DepthStencilMode::Enabled)
            {
                _depth_stencil = std::make_unique<DepthStencil>(device, _width, _height);
            }

            _view = device.create_render_target_view(_texture.texture());
        }

        // Create a render target using the specfied pre-existing texture.
        // device: The D3D device.
        // texture: The texture to use as the render target.
        // depth_mode: Whether a depth stencil should be created.
        RenderTarget::RenderTarget(const graphics::IDevice& device, const ComPtr<ID3D11Texture2D>& texture, DepthStencilMode depth_mode)
            : _texture{ texture, nullptr }
        {
            // Initialise properties from the existing texture.
            D3D11_TEXTURE2D_DESC desc;
            texture->GetDesc(&desc);
            _width = desc.Width;
            _height = desc.Height;

            if (depth_mode == DepthStencilMode::Enabled)
            {
                _depth_stencil = std::make_unique<DepthStencil>(device, _width, _height);
            }

            _view = device.create_render_target_view(_texture.texture());
        }

        RenderTarget::~RenderTarget() 
        {
        }

        // Clear the render target.
        // context: The D3D device context.
        // colour: The colour with which to clear the render target.
        void RenderTarget::clear(const ComPtr<ID3D11DeviceContext>& context, const Color& colour)
        {
            context->ClearRenderTargetView(_view.Get(), colour);
            if (_depth_stencil)
            {
                _depth_stencil->clear(context);
            }
        }

        // Set the render target as the current render target. This will also apply a viewport that matches the 
        // dimensions of the render target.
        // context: The D3D device context.
        void RenderTarget::apply(const ComPtr<ID3D11DeviceContext>& context)
        {
            D3D11_VIEWPORT viewport;
            viewport.Width = static_cast<float>(_width);
            viewport.Height = static_cast<float>(_height);
            viewport.MaxDepth = 1;
            viewport.MinDepth = 0;
            viewport.TopLeftX = 0;
            viewport.TopLeftY = 0;
            context->RSSetViewports(1, &viewport);
            context->OMSetRenderTargets(1, _view.GetAddressOf(), get_depth_stencil(_depth_stencil));
        }

        // Get the texture for the render target.
        // Returns: The texture.
        const Texture& RenderTarget::texture() const
        {
            return _texture;
        }

        // Get the render target interface for the render target.
        // Returns: The render target.
        ComPtr<ID3D11RenderTargetView> RenderTarget::render_target() const
        {
            return _view;
        }

        // Get the width of the render target in pixels.
        // Returns: The width.
        uint32_t RenderTarget::width() const
        {
            return _width;
        }

        // Get the height of the render target in pixels.
        // Returns: The height.
        uint32_t RenderTarget::height() const
        {
            return _height;
        }

        Size RenderTarget::size() const
        {
            return Size(static_cast<float>(_width), static_cast<float>(_height));
        }
    }
}
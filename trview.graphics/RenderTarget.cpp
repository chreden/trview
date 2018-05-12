#include "RenderTarget.h"

using namespace Microsoft::WRL;
using namespace DirectX::SimpleMath;

namespace trview
{
    namespace graphics
    {
        // Create a render target of the specified dimensions. The new render target will
        // have its pixels initialised to zero.
        // device: The D3D device.
        // width: The width of the new render target.
        // height: The height of the new render target.
        RenderTarget::RenderTarget(const ComPtr<ID3D11Device>& device, uint32_t width, uint32_t height)
            : _width(width), _height(height), _texture(device, width, height, Texture::Bind::RenderTarget)
        {
            device->CreateRenderTargetView(_texture.texture.Get(), nullptr, &_view);
        }

        // Clear the render target.
        // context: The D3D device context.
        // colour: The colour with which to clear the render target.
        void RenderTarget::clear(const ComPtr<ID3D11DeviceContext>& context, const Color& colour)
        {
            context->ClearRenderTargetView(_view.Get(), colour);
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
            context->OMSetRenderTargets(1, _view.GetAddressOf(), nullptr);
        }

        // Get the texture for the render target.
        // Returns: The texture.
        ComPtr<ID3D11Texture2D> RenderTarget::texture() const
        {
            return _texture.texture;
        }

        // Get the shader resource for the render target.
        // Returns: The shader resource view.
        ComPtr<ID3D11ShaderResourceView> RenderTarget::resource() const
        {
            return _texture.view;
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
    }
}
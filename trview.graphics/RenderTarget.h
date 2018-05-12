#pragma once

#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h>

namespace trview
{
    namespace graphics
    {
        // Class to create and manage render target usage.
        class RenderTarget final
        {
        public:
            // Create a render target of the specified dimensions. The new render target will
            // have its pixels initialised to zero.
            // device: The D3D device.
            // width: The width of the new render target.
            // height: The height of the new render target.
            RenderTarget(const Microsoft::WRL::ComPtr<ID3D11Device>& device, uint32_t width, uint32_t height);

            // Clear the render target.
            // context: The D3D device context.
            // colour: The colour with which to clear the render target.
            void clear(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const DirectX::SimpleMath::Color& colour);

            // Set the render target as the current render target. This will also apply a viewport that matches the 
            // dimensions of the render target.
            // context: The D3D device context.
            void apply(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);

            // Get the shader resource for the render target.
            // Returns: The shader resource view.
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource() const;

            // Get the width of the render target in pixels.
            // Returns: The width.
            uint32_t width() const;

            // Get the height of the render target in pixels.
            // Returns: The height.
            uint32_t height() const;
        private:
            Microsoft::WRL::ComPtr<ID3D11Texture2D>          _texture;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _resource;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView>   _view;
            uint32_t _width;
            uint32_t _height;
        };
    }
}


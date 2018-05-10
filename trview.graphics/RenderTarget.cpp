#include "RenderTarget.h"

#include <vector>

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        RenderTarget::RenderTarget(const ComPtr<ID3D11Device>& device, uint32_t width, uint32_t height)
            : _width(width), _height(height)
        {
            // If the size is larger than the current size, recreate the render target.
            std::vector<uint32_t> pixels(width * height, 0x00000000);

            D3D11_SUBRESOURCE_DATA srd;
            memset(&srd, 0, sizeof(srd));
            srd.pSysMem = &pixels[0];
            srd.SysMemPitch = sizeof(uint32_t) * width;

            D3D11_TEXTURE2D_DESC desc;
            memset(&desc, 0, sizeof(desc));
            desc.Width = width;
            desc.Height = height;
            desc.MipLevels = desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DEFAULT;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
            desc.CPUAccessFlags = 0;
            desc.MiscFlags = 0;

            device->CreateTexture2D(&desc, &srd, &_texture);
            device->CreateShaderResourceView(_texture.Get(), nullptr, &_resource);
            device->CreateRenderTargetView(_texture.Get(), nullptr, &_view);
        }

        void RenderTarget::clear(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const DirectX::SimpleMath::Color& colour)
        {
            context->ClearRenderTargetView(_view.Get(), colour);
        }

        void RenderTarget::apply(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context)
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

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RenderTarget::resource() const
        {
            return _resource;
        }

        uint32_t RenderTarget::width() const
        {
            return _width;
        }

        uint32_t RenderTarget::height() const
        {
            return _height;
        }
    }
}
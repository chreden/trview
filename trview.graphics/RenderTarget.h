#pragma once

#include <cstdint>
#include <d3d11.h>
#include <wrl/client.h>
#include <SimpleMath.h>

namespace trview
{
    namespace graphics
    {
        class RenderTarget final
        {
        public:
            RenderTarget(const Microsoft::WRL::ComPtr<ID3D11Device>& device, uint32_t width, uint32_t height);
            void clear(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context, const DirectX::SimpleMath::Color& colour);
            void apply(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> resource() const;
            uint32_t width() const;
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


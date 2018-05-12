#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <cstdint>

namespace trview
{
    namespace graphics
    {
        struct Texture
        {
            enum class Usage
            {
                Texture,
                RenderTarget
            };

            Texture() = default;
            Texture(const Microsoft::WRL::ComPtr<ID3D11Texture2D>& texture, const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& view);
            Texture(const Microsoft::WRL::ComPtr<ID3D11Device>& device, uint32_t width, uint32_t height, Usage usage);

            Microsoft::WRL::ComPtr<ID3D11Texture2D>          texture;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
        };
    }
}
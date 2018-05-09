#pragma once

#include <wrl/client.h>
#include <d2d1.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            struct FontTexture
            {
                Microsoft::WRL::ComPtr<ID3D11Texture2D>          texture;
                Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
                Microsoft::WRL::ComPtr<ID2D1RenderTarget>        render_target;
                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>     brush;
            };
        }
    }
}

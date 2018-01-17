#pragma once

#include <atlbase.h>
#include <d2d1.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            struct FontTexture
            {
                CComPtr<ID3D11Texture2D>          texture;
                CComPtr<ID3D11ShaderResourceView> view;
                CComPtr<ID2D1RenderTarget>        render_target;
                CComPtr<ID2D1SolidColorBrush>     brush;
            };
        }
    }
}

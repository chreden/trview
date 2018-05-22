/// @file FontTexture.h
/// @brief Holds Direct2D interfaces required to render to a D3D texture.

#pragma once

#include <wrl/client.h>
#include <d2d1.h>

namespace trview
{
    namespace graphics
    {
        /// Holds Direct2D interfaces required to render to a D3D texture.
        struct FontTexture
        {
            // The D2D render target for the font texture.
            Microsoft::WRL::ComPtr<ID2D1RenderTarget>        render_target;
            // The brush used to write the text on to the texture.
            Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>     brush;
        };
    }
}

#pragma once

#include <wrl/client.h>
#include <d2d1.h>
#include <memory>
#include <trview.graphics/RenderTarget.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            struct FontTexture
            {
                // The D3D render target for the font texture.
                std::unique_ptr<graphics::RenderTarget>          texture;
                // The D2D render target for the font texture.
                Microsoft::WRL::ComPtr<ID2D1RenderTarget>        render_target;
                // The brush used to write the text on to the texture.
                Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>     brush;
            };
        }
    }
}

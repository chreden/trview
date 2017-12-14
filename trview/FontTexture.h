#pragma once

#include <atlbase.h>
#include <d2d1.h>

#include "Texture.h"

namespace trview
{
    struct FontTexture
    {
        Texture                        texture;
        CComPtr<ID2D1RenderTarget>     render_target;
        CComPtr<ID2D1SolidColorBrush>  brush;
    };
}

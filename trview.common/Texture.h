#pragma once

#include <atlbase.h>
#include <d3d11.h>

namespace trview
{
    struct Texture
    {
        CComPtr<ID3D11Texture2D>          texture;
        CComPtr<ID3D11ShaderResourceView> view;
    };
}
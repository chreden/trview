#pragma once

namespace trview
{
    struct Texture
    {
        CComPtr<ID3D11Texture2D>          texture;
        CComPtr<ID3D11ShaderResourceView> view;
    };
}
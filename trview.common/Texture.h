#pragma once

#include <wrl/client.h>
#include <d3d11.h>

namespace trview
{
    struct Texture
    {
        Microsoft::WRL::ComPtr<ID3D11Texture2D>          texture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
    };
}
#pragma once

#include <atlbase.h>
#include <d3d11.h>

namespace trview
{
    struct IShaderStorage;

    void load_default_shaders(const CComPtr<ID3D11Device>& device, IShaderStorage& storage);
}

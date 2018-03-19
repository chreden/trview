#pragma once

#include <atlbase.h>
#include <d3d11.h>

namespace trview
{
    struct ITextureStorage;

    // Loads the textures that have been embedded in the resource file and puts them into
    // the texture storage provided.
    // device: The Direct3D device to use to load the textures.
    // storage: The ITextureStorage instance to store the textures in.
    void load_default_textures(CComPtr<ID3D11Device>, ITextureStorage& storage);
}

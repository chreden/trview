#pragma once

#include <wrl/client.h>
#include <d3d11.h>

namespace trview
{
    struct ITextureStorage;

    // Loads the textures that have been embedded in the resource file and puts them into
    // the texture storage provided.
    // device: The Direct3D device to use to load the textures.
    // storage: The ITextureStorage instance to store the textures in.
    void load_default_textures(const Microsoft::WRL::ComPtr<ID3D11Device>& device, ITextureStorage& storage);
}

#pragma once

#include <trview.graphics/Device.h>

namespace trview
{
    struct ITextureStorage;

    // Loads the textures that have been embedded in the resource file and puts them into
    // the texture storage provided.
    // device: The Direct3D device to use to load the textures.
    // storage: The ITextureStorage instance to store the textures in.
    void load_default_textures(const graphics::Device& device, ITextureStorage& storage);
}

#pragma once

namespace trview
{
    namespace graphics
    {
        struct IDevice;
    }

    struct ITextureStorage;

    // Loads the textures that have been embedded in the resource file and puts them into
    // the texture storage provided.
    // device: The Direct3D device to use to load the textures.
    // storage: The ITextureStorage instance to store the textures in.
    void load_default_textures(const graphics::IDevice& device, ITextureStorage& storage);
}

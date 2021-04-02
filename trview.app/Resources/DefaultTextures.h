#pragma once

#include <trview.app/Graphics/ITextureStorage.h>

namespace trview
{
    namespace graphics
    {
        struct IDevice;
    }

    // Loads the textures that have been embedded in the resource file and puts them into
    // the texture storage provided.
    // device: The Direct3D device to use to load the textures.
    // storage: The ITextureStorage instance to store the textures in.
    void load_default_textures(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<ITextureStorage>& storage);
}

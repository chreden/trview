#include "DefaultTextures.h"
#include <trview.common/Resources.h>
#include <trview.graphics/IDevice.h>
#include <trview.app/Graphics/ITextureStorage.h>
#include <trview.common/Strings.h>

using namespace Microsoft::WRL;

namespace trview
{
    // Loads the textures that have been embedded in the resource file and puts them into
    // the texture storage provided.
    // device: The Direct3D device to use to load the textures.
    // storage: The ITextureStorage instance to store the textures in.
    void load_default_textures(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<ITextureStorage>& storage)
    {
        for (const auto& name : get_resource_names_of_type(L"TEXTURE"))
        {
            storage->store(to_utf8(name), load_texture_from_resource(*device, name));
        }
    }
}

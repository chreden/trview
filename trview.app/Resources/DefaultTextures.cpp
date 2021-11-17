#include "DefaultTextures.h"
#include "resource.h"
#include <trview.common/Resources.h>
#include <trview.graphics/Device.h>
#include <trview.app/Graphics/ITextureStorage.h>

using namespace Microsoft::WRL;

namespace trview
{
    // Loads the textures that have been embedded in the resource file and puts them into
    // the texture storage provided.
    // device: The Direct3D device to use to load the textures.
    // storage: The ITextureStorage instance to store the textures in.
    void load_default_textures(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<ITextureStorage>& storage)
    {
        // Load some sort of manifest that contains the files to load.
        // For each texture, load it with the given key.
        Resource texture_list = get_resource_memory(IDR_TEXTURE_LIST, L"TEXT");
        
        auto contents = std::string(texture_list.data, texture_list.data + texture_list.size);
        std::stringstream stream(contents);

        while (!stream.eof())
        {
            std::string key;
            int resource_id = 0;
            if (!(stream >> key >> resource_id))
            {
                break;
            }

            storage->store(key, load_texture_from_resource(*device, resource_id));

            if (!std::getline(stream, key))
            {
                break;
            }
        }
    }
}

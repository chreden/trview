#include "stdafx.h"
#include "DefaultTextures.h"
#include "resource.h"

#include <trview.app/Graphics/ITextureStorage.h>
#include "ResourceHelper.h"

#include <external/DirectXTK/Inc/WICTextureLoader.h>
#include <sstream>

using namespace Microsoft::WRL;

namespace trview
{
    namespace
    {
        // Load a specific texture with the specified ID from the embedded resource file.
        // device: The Direct3D device to use to load the textures.
        // resource_id: The integer ID of the texture in the resource file.
        // Returns: The texture loaded from the resource.
        graphics::Texture load_texture_from_resource(const graphics::Device& device, int resource_id)
        {
            ComPtr<ID3D11Resource> resource;
            ComPtr<ID3D11ShaderResourceView> view;

            auto resource_memory = get_resource_memory(resource_id, L"PNG");
            DirectX::CreateWICTextureFromMemory(device.device().Get(), resource_memory.data, resource_memory.size, &resource, &view);

            if (!resource)
            {
                std::string error("Could not load embedded texture with ID '" + std::to_string(resource_id) + "'");
                throw std::exception(error.c_str());
            }

            // Get the correct interface for a texture from the loaded resource.
            ComPtr<ID3D11Texture2D> texture;
            resource.As(&texture);
            return graphics::Texture{ texture, view };
        }
    }

    // Loads the textures that have been embedded in the resource file and puts them into
    // the texture storage provided.
    // device: The Direct3D device to use to load the textures.
    // storage: The ITextureStorage instance to store the textures in.
    void load_default_textures(const graphics::Device& device, ITextureStorage& storage)
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

            storage.store(key, load_texture_from_resource(device, resource_id));

            if (!std::getline(stream, key))
            {
                break;
            }
        }
    }
}

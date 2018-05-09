#include "stdafx.h"
#include "DefaultTextures.h"
#include "resource.h"

#include "ITextureStorage.h"

#include <external/DirectXTK/Inc/WICTextureLoader.h>
#include <sstream>

namespace trview
{
    namespace
    {
        struct Resource
        {
            uint8_t* data;
            uint32_t size;
        };

        // Get the data for the specified resource.
        // resource_id: The integer identifier of the resource to load.
        // resource_type: The type of resource to load.
        // Returns: The resource.
        Resource get_resource_memory(int resource_id, const wchar_t* resource_type)
        {
            HMODULE module = GetModuleHandle(NULL);
            HRSRC resource = FindResource(module, MAKEINTRESOURCE(resource_id), resource_type);
            HGLOBAL memory = LoadResource(module, resource);
            DWORD size = SizeofResource(module, resource);
            LPVOID data = LockResource(memory);
            return Resource{ static_cast<uint8_t*>(data), static_cast<uint32_t>(size) };
        }

        // Load a specific texture with the specified ID from the embedded resource file.
        // device: The Direct3D device to use to load the textures.
        // resource_id: The integer ID of the texture in the resource file.
        // Returns: The texture loaded from the resource.
        Texture load_texture_from_resource(const Microsoft::WRL::ComPtr<ID3D11Device>& device, int resource_id)
        {
            Microsoft::WRL::ComPtr<ID3D11Resource> resource;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;

            auto resource_memory = get_resource_memory(resource_id, L"PNG");
            DirectX::CreateWICTextureFromMemory(device.Get(), resource_memory.data, resource_memory.size, &resource, &view);

            if (!resource)
            {
                std::string error("Could not load embedded texture with ID '" + std::to_string(resource_id) + "'");
                throw std::exception(error.c_str());
            }

            // Get the correct interface for a texture from the loaded resource.
            Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
            resource.As(&texture);
            return Texture{ texture, view };
        }
    }

    // Loads the textures that have been embedded in the resource file and puts them into
    // the texture storage provided.
    // device: The Direct3D device to use to load the textures.
    // storage: The ITextureStorage instance to store the textures in.
    void load_default_textures(const Microsoft::WRL::ComPtr<ID3D11Device>& device, ITextureStorage& storage)
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

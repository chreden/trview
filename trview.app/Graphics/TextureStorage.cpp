#include "TextureStorage.h"
#include <trview.common/Strings.h>

namespace trview
{
    ITextureStorage::~ITextureStorage()
    {
    }

    TextureStorage::TextureStorage(const std::shared_ptr<graphics::IDevice>& device)
        : _device(device)
    {
    }

    graphics::Texture TextureStorage::coloured(uint32_t colour) const
    {
        return graphics::Texture(*_device, 1, 1, std::vector<uint32_t>(1, colour));
    }

    graphics::Texture TextureStorage::lookup(const std::string& key) const
    {
        auto found = _textures.find(to_lowercase(key));
        if (found == _textures.end())
        {
            return graphics::Texture();
        }
        return found->second;
    }

    void TextureStorage::store(const std::string& key, const graphics::Texture& texture)
    {
        _textures.insert({ to_lowercase(key), texture });
    }
}

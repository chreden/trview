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
        _untextured_texture = coloured(0xffffffff);
        // Generate the TRLE texture.
        std::vector<uint32_t> pixels(256 * 256, 0xffffffff);
        for (int x = 0; x < 256; ++x)
        {
            pixels[x] = 0xff000000;
            pixels[x + 255 * 256] = 0xff000000;
        }
        for (int y = 0; y < 256; ++y)
        {
            pixels[y * 256] = 0xff000000;
            pixels[y * 256 + 255] = 0xff000000;
        }
        _geometry_texture = graphics::Texture(*_device, 256, 256, pixels);
    }

    void TextureStorage::add_texture(const std::vector<uint32_t>& pixels, uint32_t width, uint32_t height)
    {
        _tiles.emplace_back(*_device, width, height, pixels);
    }

    graphics::Texture TextureStorage::coloured(uint32_t colour) const
    {
        return graphics::Texture(*_device, 1, 1, std::vector<uint32_t>(1, colour));
    }

    graphics::Texture TextureStorage::geometry_texture() const
    {
        return _geometry_texture;
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

    uint32_t TextureStorage::num_textures() const
    {
        return static_cast<uint32_t>(_tiles.size());
    }

    void TextureStorage::store(const std::string& key, const graphics::Texture& texture)
    {
        _textures.insert({ to_lowercase(key), texture });
    }

    graphics::Texture TextureStorage::texture(uint32_t tile_index) const
    {
        return _tiles[tile_index];
    }

    graphics::Texture TextureStorage::untextured() const
    {
        return _untextured_texture;
    }
}

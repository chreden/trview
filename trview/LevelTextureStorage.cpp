#include "stdafx.h"
#include "LevelTextureStorage.h"
#include "TextureStorage.h"

namespace trview
{
    LevelTextureStorage::LevelTextureStorage(const graphics::Device& device, const trlevel::ILevel& level)
        : _device(device), _level(level), _texture_storage(std::make_unique<TextureStorage>(device))
    {
        for (uint32_t i = 0; i < level.num_textiles(); ++i)
        {
            std::vector<uint32_t> data = level.get_textile(i);
            _tiles.emplace_back(device, 256, 256, data);
        }

        // Copy object textures locally from the level.
        for (uint32_t i = 0; i < level.num_object_textures(); ++i)
        {
            _object_textures.push_back(level.get_object_texture(i));
        }
    }

    graphics::Texture LevelTextureStorage::texture(uint32_t tile_index) const
    {
        return _tiles[tile_index];
    }

    graphics::Texture LevelTextureStorage::coloured(uint32_t colour) const
    {
        return _texture_storage->coloured(colour);
    }

    graphics::Texture LevelTextureStorage::untextured() const
    {
        if (!_untextured_texture.has_content())
        {
            _untextured_texture = coloured(0xffffffff);
        }
        return _untextured_texture;
    }

    DirectX::SimpleMath::Vector2 LevelTextureStorage::uv(uint32_t texture_index, uint32_t uv_index) const
    {
        using namespace DirectX::SimpleMath;
        const auto& vert = _object_textures[texture_index].Vertices[uv_index];
        return Vector2(static_cast<float>(vert.Xpixel + vert.Xcoordinate), static_cast<float>(vert.Ypixel + vert.Ycoordinate)) / 255.0f;
    }

    uint32_t LevelTextureStorage::tile(uint32_t texture_index) const
    {
        return _object_textures[texture_index].TileAndFlag & 0x7FFF;
    }

    uint32_t LevelTextureStorage::num_tiles() const
    {
        return static_cast<uint32_t>(_tiles.size());
    }

    uint16_t LevelTextureStorage::attribute(uint32_t texture_index) const
    {
        return _object_textures[texture_index].Attribute;
    }

    DirectX::SimpleMath::Color LevelTextureStorage::palette_from_texture(uint32_t texture) const
    {
        using namespace DirectX::SimpleMath;
        auto palette = _level.get_palette_entry(texture & 0xff, texture >> 8);
        return Color(palette.Red / 255.f, palette.Green / 255.f, palette.Blue / 255.f, 1.0f);
    }

    graphics::Texture LevelTextureStorage::lookup(const std::string&) const
    {
        return graphics::Texture();
    }

    void LevelTextureStorage::store(const std::string&, const graphics::Texture&)
    {
    }
}
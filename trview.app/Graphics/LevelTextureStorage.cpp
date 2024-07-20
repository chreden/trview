#include "LevelTextureStorage.h"
#include "TextureStorage.h"

namespace trview
{
    ILevelTextureStorage::~ILevelTextureStorage()
    {
    }

    LevelTextureStorage::LevelTextureStorage(const std::shared_ptr<graphics::IDevice>& device, std::unique_ptr<ITextureStorage> texture_storage)
        : _device(device), _texture_storage(std::move(texture_storage))
    {
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

    void LevelTextureStorage::determine_texture_mode()
    {
        for (const auto& object_texture : _object_textures)
        {
            for (const auto& vert : object_texture.Vertices)
            {
                if ((vert.x_frac > 1 && vert.x_frac < 255) ||
                    (vert.y_frac > 1 && vert.y_frac < 255))
                {
                    _texture_mode = TextureMode::Custom;
                    return;
                }
            }
        }
    }

    graphics::Texture LevelTextureStorage::texture(uint32_t tile_index) const
    {
        return _tiles[tile_index];
    }

    graphics::Texture LevelTextureStorage::opaque_texture(uint32_t tile_index) const
    {
        return _opaque_tiles[tile_index];
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
        if (texture_index >= _object_textures.size())
        {
            return Vector2::Zero;
        }

        const auto& vert = _object_textures[texture_index].Vertices[uv_index];

        if (_texture_mode == TextureMode::Official)
        {
            return Vector2(static_cast<float>(vert.x_whole + static_cast<int8_t>(vert.x_frac)), static_cast<float>(vert.y_whole + static_cast<int8_t>(vert.y_frac))) / 255.0f;
        }
        
        float x = static_cast<float>(vert.x_whole) + (vert.x_frac / 256.0f);
        float y = static_cast<float>(vert.y_whole) + (vert.y_frac / 256.0f);
        return Vector2(x, y) / 256.0f;
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
        if (_version > trlevel::LevelVersion::Tomb1)
        {
            return _palette[texture >> 8];
        }

        if (_platform == trlevel::Platform::PSX)
        {
            if (auto level = _level.lock())
            {
                const auto colour = level->get_palette_entry(texture);
                return DirectX::SimpleMath::Color(colour.Red / 255.f, colour.Green / 255.f, colour.Blue / 255.f, 1.0f);
            }
            return Colour::Black;
        }

        return _palette[texture & 0xff];
    }

    graphics::Texture LevelTextureStorage::lookup(const std::string&) const
    {
        return graphics::Texture();
    }

    void LevelTextureStorage::store(const std::string&, const graphics::Texture&)
    {
    }

    graphics::Texture LevelTextureStorage::geometry_texture() const
    {
        return _geometry_texture;
    }

    uint32_t LevelTextureStorage::num_object_textures() const
    {
        return static_cast<uint32_t>(_object_textures.size());
    }

    void LevelTextureStorage::load(const std::shared_ptr<trlevel::ILevel>& level)
    {
        _version = level->get_version();
        _level = level;
        _platform = level->platform();

        // Copy object textures locally from the level.
        for (uint32_t i = 0; i < level->num_object_textures(); ++i)
        {
            _object_textures.push_back(level->get_object_texture(i));
        }

        if (_version < trlevel::LevelVersion::Tomb4)
        {
            using namespace DirectX::SimpleMath;
            for (uint32_t i = 0; i < 256; ++i)
            {
                auto entry = level->get_palette_entry(i);
                _palette[i] = Color(entry.Red / 255.f, entry.Green / 255.f, entry.Blue / 255.f, 1.0f);
            }
        }

        determine_texture_mode();
    }

    void LevelTextureStorage::add_textile(const std::vector<uint32_t>& textile)
    {
        _tiles.emplace_back(*_device, 256, 256, textile);
        auto opaque = textile;
        for (auto& d : opaque)
        {
            d |= 0xff000000;
        }
        _opaque_tiles.emplace_back(*_device, 256, 256, opaque);
    }
}
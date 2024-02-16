#include "LevelTextureStorage.h"
#include "TextureStorage.h"
#include <external/DirectXTK/Inc/DDSTextureLoader.h>
#include <unordered_map>

namespace trview
{
    ILevelTextureStorage::~ILevelTextureStorage()
    {
    }

    LevelTextureStorage::LevelTextureStorage(const std::shared_ptr<graphics::IDevice>& device, std::unique_ptr<ITextureStorage> texture_storage, const std::shared_ptr<trlevel::ILevel>& level, const std::shared_ptr<IFiles>&)
        : _texture_storage(std::move(texture_storage)), _version(level->get_version()), _level(level), _platform(level->platform())
    {
        for (uint32_t i = 0; i < level->num_textiles(); ++i)
        {
            std::vector<uint32_t> data = level->get_textile(i);
            _tiles.emplace_back(*device, 256, 256, data);
            for (auto& d : data)
            {
                d |= 0xff000000;
            }
            _opaque_tiles.emplace_back(*device, 256, 256, data);
        }

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
        _geometry_texture = graphics::Texture(*device, 256, 256, pixels);

        determine_texture_mode();
        generate_remastered_textures(device, level);
    }

    std::string get_fn(const std::string& filename)
    {
        const auto last_fs = filename.find_last_of('/');
        const auto last_bs = filename.find_last_of('\\');
        const auto last = last_fs == filename.npos ? last_bs : last_fs;
        return filename.substr(last + 1);
    }

    void LevelTextureStorage::generate_remastered_textures(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<trlevel::ILevel>& level)
    {
        // Build up a set of created textures - then make the new tiles.
        std::unordered_map<std::string, graphics::Texture> sub_tiles;
        std::vector<graphics::Texture> tiles;

        for (uint32_t i = 0; i < _tiles.size(); ++i)
        {
            auto tile = graphics::create_texture(*device, 2048, 2048, Colour::White, DXGI_FORMAT_BC7_UNORM);

            auto new_textile = tile.texture();
            auto textile = level->get_remastered_textile(i);
            int count = 0;

            for (const auto& entry : textile.paths)
            {
                auto existing = sub_tiles.find(entry);
                if (existing == sub_tiles.end())
                {
                    Microsoft::WRL::ComPtr<ID3D11Resource> tex;
                    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;

                    HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
                        device->device().Get(),
                        device->context().Get(),
                        to_utf16(entry).c_str(),
                        0,
                        D3D11_USAGE_DEFAULT,
                        D3D11_BIND_SHADER_RESOURCE,
                        0,
                        0,
                        DirectX::DDS_LOADER_DEFAULT,
                        tex.GetAddressOf(),
                        srv.GetAddressOf(),
                        nullptr);
                    if (S_OK == hr)
                    {
                        Microsoft::WRL::ComPtr<ID3D11Texture2D> image;
                        tex->QueryInterface<ID3D11Texture2D>(image.GetAddressOf());
                        graphics::Texture texture { image, srv };
                        D3D11_TEXTURE2D_DESC desc;
                        image->GetDesc(&desc);
                        texture.set_name(get_fn(entry));
                        sub_tiles[entry] = texture;
                    }
                }

                existing = sub_tiles.find(entry);
                if (existing != sub_tiles.end())
                {
                    // const int y = (count / 4) * 512;
                    // const int x = 1536 - (512 * (count - (count / 4) * 4));
                    // device->context()->CopySubresourceRegion(new_textile.Get(), 0, x, y, 0, existing->second.texture().Get(), 0, nullptr);
                    _remastered_textures.push_back(existing->second);
                }

                ++count;
            }

            // _remastered_textures.push_back(tile);
        }
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
        if (tile_index < _remastered_textures.size())
        {
            // return _remastered_textures[tile_index];
        }
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

    graphics::Texture LevelTextureStorage::remastered_texture(uint32_t texture_index) const
    {
        return _remastered_textures[texture_index];
    }

    uint32_t LevelTextureStorage::num_remastered_textures() const
    {
        return static_cast<uint32_t>(_remastered_textures.size());
    }
}
#include "LevelTextureStorage.h"
#include "TextureStorage.h"
#include <ranges>

namespace trview
{
    namespace
    {
        D3D11_BOX to_box(const trlevel::tr_object_texture& object_texture)
        {
            uint8_t left = UINT8_MAX;
            uint8_t top = UINT8_MAX;
            uint8_t right = 0;
            uint8_t bottom = 0;

            for (const auto& v : object_texture.Vertices)
            {
                left = std::min(left, v.x_whole);
                top = std::min(top, v.y_whole);
                right = std::max(right, v.x_whole);
                bottom = std::max(bottom, v.y_whole);
            }

            return D3D11_BOX { .left = left, .top = top, .front = 0, .right = static_cast<uint32_t>(right) + 1, .bottom = static_cast<uint32_t>(bottom) + 1, .back = 1 };
        }
    }

    ILevelTextureStorage::~ILevelTextureStorage()
    {
    }

    LevelTextureStorage::LevelTextureStorage(const std::shared_ptr<graphics::IDevice>& device, std::unique_ptr<ITextureStorage> texture_storage)
        : _device(device), _texture_storage(std::move(texture_storage))
    {
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

    void LevelTextureStorage::add_texture(const std::vector<uint32_t>& pixels, uint32_t width, uint32_t height)
    {
        _texture_storage->add_texture(pixels, width, height);
    }

    graphics::Texture LevelTextureStorage::texture(uint32_t tile_index) const
    {
        return _texture_storage->texture(tile_index);
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
        return _texture_storage->untextured();
    }

    DirectX::SimpleMath::Vector2 LevelTextureStorage::uv(uint32_t texture_index, uint32_t uv_index) const
    {
        using namespace DirectX::SimpleMath;
        if (texture_index >= _object_textures.size())
        {
            return Vector2::Zero;
        }

        auto vert = _object_textures[texture_index].Vertices[uv_index];

        const auto found = _animated_uv_textures.find(texture_index);
        if (found != _animated_uv_textures.end())
        {
            vert = found->second.object_texture.Vertices[uv_index];
        }

        const auto found_repl = _texture_replacements.find(texture_index);
        if (found_repl != _texture_replacements.end())
        {
            return found_repl->second.uvs[uv_index];
        }

        if (_texture_mode == TextureMode::Official)
        {
            float u = static_cast<float>(vert.x_whole);
            float v = static_cast<float>(vert.y_whole);

            if (vert.x_frac == 1 || vert.x_frac == 0)
            {
                u += 1;
            }
            else if (vert.x_frac == 255)
            {
                u -= 1;
            }

            if (vert.y_frac == 1 || vert.y_frac == 0)
            {
                v += 1;
            }
            else if (vert.y_frac == 255)
            {
                v -= 1;
            }

            return Vector2(u, v) / 256.0f;
        }
        
        float x = static_cast<float>(vert.x_whole) + (vert.x_frac / 256.0f);
        float y = static_cast<float>(vert.y_whole) + (vert.y_frac / 256.0f);
        return Vector2(x, y) / 256.0f;
    }

    uint32_t LevelTextureStorage::tile(uint32_t texture_index) const
    {
        const auto found = _animated_uv_textures.find(texture_index);
        if (found != _animated_uv_textures.end())
        {
            return found->second.new_tile;
        }

        const auto found_repl = _texture_replacements.find(texture_index);
        if (found_repl != _texture_replacements.end())
        {
            return found_repl->second.tile;
        }

        if (texture_index < _object_textures.size())
        {
            return _object_textures[texture_index].TileAndFlag & 0x7FFF;
        }
        return 0;
    }

    uint32_t LevelTextureStorage::num_textures() const
    {
        return _texture_storage->num_textures();
    }

    uint32_t LevelTextureStorage::num_tiles() const
    {
        return _texture_storage->num_textures();
    }

    uint16_t LevelTextureStorage::attribute(uint32_t texture_index) const
    {
        if (texture_index < _object_textures.size())
        {
            return _object_textures[texture_index].Attribute;
        }
        return 0;
    }

    DirectX::SimpleMath::Color LevelTextureStorage::palette_from_texture(uint32_t texture) const
    {
        if (_platform_and_version.version > trlevel::LevelVersion::Tomb1)
        {
            return _palette[texture >> 8];
        }

        if (_platform_and_version.platform == trlevel::Platform::PSX)
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
        return _texture_storage->geometry_texture();
    }

    uint32_t LevelTextureStorage::num_object_textures() const
    {
        return static_cast<uint32_t>(_object_textures.size());
    }

    trlevel::PlatformAndVersion LevelTextureStorage::platform_and_version() const
    {
        return _platform_and_version;
    }

    void LevelTextureStorage::load(const std::shared_ptr<trlevel::ILevel>& level)
    {
        _platform_and_version = level->platform_and_version();
        _level = level;

        _object_textures = level->object_textures();

        generate_replacement_textures();

        uint32_t sequence_index = 0;
        for (const auto& sequence : level->animated_textures())
        {
            for (const auto& entry : sequence)
            {
                if (sequence_index < level->animated_texture_uv_count())
                {
                    if (_animated_uv_textures.find(entry) == _animated_uv_textures.end())
                    {
                        _animated_uv_textures.insert({ entry, {} });
                    }
                }
                else
                {
                    _animated_textures[static_cast<uint32_t>(entry)] = sequence | std::ranges::to<std::vector<uint32_t>>();
                }
            }
            ++sequence_index;
        }

        for (auto& uv_texture : _animated_uv_textures)
        {
            const auto& object_texture = _object_textures[uv_texture.first];
            const auto& tile = _opaque_tiles[object_texture.TileAndFlag & 0x7FFF];

            // 1. Copy the contents of the object texture to new texture.
            auto source_texture = tile.texture();

            D3D11_BOX box = to_box(object_texture);
            const uint32_t width = box.right - box.left;
            const uint32_t height = box.bottom - box.top;

            std::vector<uint32_t> blank;
            blank.resize(width * height, 0xffffffff);
            _texture_storage->add_texture(blank, width, height);
            uint32_t new_index = _texture_storage->num_textures() - 1;
            auto dest_texture = _texture_storage->texture(new_index);

            Microsoft::WRL::ComPtr<ID3D11Resource> source_resource;
            Microsoft::WRL::ComPtr<ID3D11Resource> dest_resource;
            source_texture.As<ID3D11Resource>(&source_resource);
            dest_texture.texture().As<ID3D11Resource>(&dest_resource);

            _device->context()->CopySubresourceRegion(dest_resource.Get(), 0, 0, 0, 0, source_resource.Get(), 0, &box);

            // 2. Store altered UVs
            // 3. Map object texture to new tile number
            trlevel::tr_object_texture remapped_texture = object_texture;
            for (auto& v : remapped_texture.Vertices)
            {
                v.x_whole = static_cast<uint8_t>((static_cast<float>(v.x_whole - box.left) / (width - 1)) * 255);
                v.y_whole = static_cast<uint8_t>((static_cast<float>(v.y_whole - box.top) / (height - 1)) * 255);
            }
            uv_texture.second.object_texture = remapped_texture;
            uv_texture.second.new_tile = new_index;
        }

        if (_platform_and_version.version < trlevel::LevelVersion::Tomb4)
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

    void LevelTextureStorage::add_textile(const std::vector<uint32_t>& textile, uint32_t width, uint32_t height)
    {
        _texture_storage->add_texture(textile, width, height);
        auto opaque = textile;
        for (auto& d : opaque)
        {
            d |= 0xff000000;
        }
        _opaque_tiles.emplace_back(*_device, width, height, opaque);
        _source_textures.push_back({ .width = width, .height = height, .bytes = textile });
    }

    Triangle::AnimationMode LevelTextureStorage::animation_mode(uint32_t texture_index) const
    {
        if (_animated_textures.find(texture_index) != _animated_textures.end())
        {
            return Triangle::AnimationMode::Swap;
        }
        return Triangle::AnimationMode::None;
    }

    std::vector<uint32_t> LevelTextureStorage::animated_texture(uint32_t texture_index) const
    {
        auto found = _animated_textures.find(texture_index);
        if (found == _animated_textures.end())
        {
            return {};
        }

        auto sequence = found->second;
        auto inner = std::ranges::find(sequence, texture_index);
        std::ranges::rotate(sequence, inner);
        return sequence;
    }

    void LevelTextureStorage::generate_replacement_textures()
    {
        using namespace DirectX::SimpleMath;
        for (auto i = 0; i < _object_textures.size(); ++i)
        {
            const trlevel::tr_object_texture& ot = _object_textures[i];

            auto [min_x, max_x] = std::ranges::minmax(ot.Vertices | std::views::transform([](auto&& v) { return v.x_whole; }));
            auto [min_y, max_y] = std::ranges::minmax(ot.Vertices | std::views::transform([](auto&& v) { return v.y_whole; }));

            const uint32_t width = max_x - min_x + 1;
            const uint32_t height = max_y - min_y + 1;

            std::vector<uint32_t> output_texture;
            output_texture.resize(width * height, 0xffff00ff);

            const auto& source_texture = _source_textures[ot.TileAndFlag & 0x7fff];

            for (uint32_t y = 0; y < height; ++y)
            {
                memcpy(
                    &output_texture[y * width],
                    &source_texture.bytes[(min_y + y) * source_texture.width + min_x],
                    sizeof(uint32_t) * width);
            }

            TextureReplacement repl =
            {
                .uvs = ot.Vertices | std::views::transform([&](auto&& v) -> Vector2
                {
                    return Vector2(
                        (static_cast<float>(v.x_whole - min_x) / (width - 1)),
                        (static_cast<float>(v.y_whole - min_y) / (height - 1)));
                }) | std::ranges::to<std::vector>(),
                .tile = _texture_storage->num_textures()
            };

            _texture_replacements[static_cast<uint32_t>(i)] = repl;
            add_textile(output_texture, width, height);
        }

        _source_textures = {};
    }
}
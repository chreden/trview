#include "Level_psx.h"
#include "Level_common.h"
#include "Level.h"

#include <format>

namespace trlevel
{
    std::vector<tr_model> read_models_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading models");
        log_file(activity, file, "Reading models");
        auto models = convert_models(read_vector<uint32_t, tr_model_psx>(file));
        log_file(activity, file, std::format("Read {} models", models.size()));
        return models;
    }

    tr_colour4 Level::colour_from_object_texture(uint32_t texture) const
    {
        if (texture >= _object_textures_psx.size())
        {
            return tr_colour4{ .Red = 0, .Green = 0, .Blue = 0 };
        }

        const auto& object_texture = _object_textures_psx[texture];
        if (object_texture.Tile >= _textile4.size() ||
            object_texture.Clut >= _clut.size())
        {
            return tr_colour4{ .Red = 0, .Green = 0, .Blue = 0 };
        }

        const auto& tile = _textile4[object_texture.Tile];
        const auto& clut = _clut[object_texture.Clut];

        const auto pixel = object_texture.x0 + object_texture.y0 * 256;
        const auto index = tile.Tile[pixel / 2];
        const auto colour = clut.Colour[object_texture.x0 % 2 ? index.b : index.a];

        const float r = colour.Red / 31.0f;
        const float g = colour.Green / 31.0f;
        const float b = colour.Blue / 31.0f;

        return tr_colour4
        {
            .Red = static_cast<uint8_t>(std::min(1.0f, r) * 255),
            .Green = static_cast<uint8_t>(std::min(1.0f, g) * 255),
            .Blue = static_cast<uint8_t>(std::min(1.0f, b) * 255)
        };
    }

    uint16_t Level::convert_textile4(uint16_t tile, uint16_t clut_id)
    {
        // Check if we've already converted this tile + clut
        for (auto i = _converted_t16.begin(); i < _converted_t16.end(); ++i)
        {
            if (i->first == tile && i->second == clut_id)
            {
                return static_cast<uint16_t>(std::distance(_converted_t16.begin(), i));
            }
        }
        // If not, create new conversion
        _converted_t16.push_back(std::make_pair(tile, clut_id));

        tr_textile16& tile16 = _textile16.emplace_back();
        if (tile < _textile4.size() && clut_id < _clut.size())
        {
            const tr_textile4& tile4 = _textile4[tile];
            const tr_clut& clut = _clut[clut_id];
            for (int x = 0; x < 256; ++x)
            {
                for (int y = 0; y < 256; ++y)
                {
                    const std::size_t pixel = (y * 256 + x);
                    const tr_colorindex4& index = tile4.Tile[pixel / 2];
                    const tr_rgba5551& colour = clut.Colour[(x % 2) ? index.b : index.a];
                    tile16.Tile[pixel] = (colour.Alpha << 15) | (colour.Red << 10) | (colour.Green << 5) | colour.Blue;
                }
            }
        }
        else
        {
            memset(tile16.Tile, 0, sizeof(tile16.Tile));
        }

        _num_textiles = static_cast<uint32_t>(_textile16.size());
        return static_cast<uint16_t>(_textile16.size() - 1);
    };
}
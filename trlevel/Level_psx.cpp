#include "Level_psx.h"
#include "Level_common.h"
#include "Level.h"
#include "IPack.h"

#include <trview.common/Algorithms.h>

#include <format>
#include <ranges>

namespace trlevel
{
    namespace
    {
        template <typename T>
        void write(std::basic_ospanstream<uint8_t>& file, const T& value)
        {
            file.write(reinterpret_cast<const uint8_t*>(&value), sizeof(value));
        }

        constexpr double load_sample(int value, int shift_factor)
        {
            if (value & 0x8000)
            {
                value |= 0xffff0000;
            }
            return static_cast<double>(value >> shift_factor);
        }
    }

    std::vector<tr_room_vertex> convert_psx_vertex_lighting(std::vector<tr_room_vertex> vertices)
    {
        return vertices | std::views::transform([](auto&& v) -> tr_room_vertex
            {
                return  { .vertex = v.vertex, .lighting = static_cast<int16_t>(8192 - (v.lighting << 5)) };
            }) | std::ranges::to<std::vector>();
    }

    /// <summary>
    /// Based on vag2wav from http://unhaut.epizy.com/psxsdk/
    /// </summary>
    std::vector<uint8_t> convert_vag_to_wav(const std::vector<uint8_t>& bytes, uint32_t sample_frequency)
    {
        std::span span{ &bytes[0], bytes.size() };
        std::basic_ispanstream<uint8_t> in_stream{ span };
        in_stream.exceptions(std::istream::failbit | std::istream::badbit | std::istream::eofbit);
        in_stream.seekg(16, std::ios::beg);

        double s_1 = 0.0;
        double s_2 = 0.0;
        double samples[28];
        constexpr double f[5][2] = { { 0.0, 0.0 },{ 60.0 / 64.0,  0.0 }, {  115.0 / 64.0, -52.0 / 64.0 }, {   98.0 / 64.0, -55.0 / 64.0 }, {  122.0 / 64.0, -60.0 / 64.0 } };

        // Uncompressed is usually about 3.33 times larger
        std::vector<uint8_t> results;
        results.resize(static_cast<uint32_t>(3.5 * bytes.size()));

        std::span out_span{ &results[0], results.size() };
        std::basic_ospanstream<uint8_t> out_stream{ out_span };
        out_stream.exceptions(std::istream::failbit | std::istream::badbit | std::istream::eofbit);

        out_stream << "RIFF";
        out_stream.seekp(4, std::ios::cur);
        out_stream << "WAVEfmt ";
        write(out_stream, 16);
        write<uint16_t>(out_stream, 1);
        write<uint16_t>(out_stream, 1);
        write<uint32_t>(out_stream, sample_frequency);
        write<uint32_t>(out_stream, sample_frequency * 2);
        write<uint16_t>(out_stream, 2);
        write<uint16_t>(out_stream, 16);
        out_stream << "data";
        out_stream.seekp(4, std::ios::cur);

        while (static_cast<uint32_t>(in_stream.tellg()) + 48 < static_cast<uint32_t>(bytes.size()))
        {
            int predict_nr = read<char>(in_stream);
            int shift_factor = predict_nr & 0xf;
            predict_nr >>= 4;
            if (read<uint8_t>(in_stream) == 7)
            {
                break;
            }

            for (int i = 0; i < 28; i += 2)
            {
                const int d = read<uint8_t>(in_stream);
                samples[i] = load_sample((d & 0xf) << 12, shift_factor);
                samples[i + 1] = load_sample((d & 0xf0) << 8, shift_factor);
            }

            for (int i = 0; i < 28; i++)
            {
                samples[i] = samples[i] + s_1 * f[predict_nr][0] + s_2 * f[predict_nr][1];
                s_2 = s_1;
                s_1 = samples[i];
                write<int16_t>(out_stream, static_cast<int16_t>(samples[i] + 0.5));
            }
        }

        const uint32_t file_size = static_cast<uint32_t>(out_stream.tellp());
        out_stream.seekp(4, std::ios::beg);
        write<uint32_t>(out_stream, file_size - 8);
        out_stream.seekp(40, std::ios::beg);
        write<uint32_t>(out_stream, file_size - 44);
        results.resize(file_size);
        return results;
    }

    bool is_supported_tr4_psx_version(int32_t version)
    {
        return trview::equals_any(version, -111, -120, -126, -129);
    }

    bool is_supported_tr5_psx_version(int32_t version)
    {
        return trview::equals_any(version, -206, -214, -223, -224, -225);
    }

    std::vector<tr_model> read_models_psx(trview::Activity& activity, std::basic_ispanstream<uint8_t>& file, const ILevel::LoadCallbacks& callbacks)
    {
        callbacks.on_progress("Reading models");
        log_file(activity, file, "Reading models");
        auto models = convert_models(read_vector<uint32_t, tr_model_psx>(file));
        log_file(activity, file, std::format("Read {} models", models.size()));
        return models;
    }

    void Level::read_sounds_tr1_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const ILevel::LoadCallbacks& callbacks, uint32_t sample_frequency)
    {
        callbacks.on_progress("Reading sounds");
        log_file(activity, file, "Reading sounds");
        const uint16_t num_samples = read<uint16_t>(file);
        skip(file, 2062 + num_samples * 512);

        const uint32_t sample_start = static_cast<uint32_t>(file.tellg());
        const auto sample_sizes = read_vector<uint16_t>(file, num_samples)
            | std::views::transform([](uint16_t v) -> uint32_t { return static_cast<uint32_t>(v) * 8; })
            | std::ranges::to<std::vector>();

        file.seekg(sample_start + 510, std::ios::beg);
        skip(file, 4);

        for (uint32_t s = 0; s < sample_sizes.size(); ++s)
        {
            callbacks.on_progress(std::format("Loading sound {} of {}", s, sample_sizes.size()));
            log_file(activity, file, std::format("Loading sound {} of {}", s, sample_sizes.size()));
            if (sample_sizes[s] > 0)
            {
                _sound_samples.push_back(convert_vag_to_wav(read_vector<uint8_t>(file, sample_sizes[s]), sample_frequency));
            }
        }

        log_file(activity, file, std::format("Read {} sounds", sample_sizes.size()));
    }

    void Level::read_sounds_psx(std::basic_ispanstream<uint8_t>& file, trview::Activity& activity, const ILevel::LoadCallbacks& callbacks, uint32_t sample_frequency)
    {
        callbacks.on_progress("Reading sounds");
        log_file(activity, file, "Reading sounds");

        const auto sound_offsets = read_vector<uint32_t, uint32_t>(file);
        const auto sound_data = read_vector<uint32_t, byte>(file);

        for (uint32_t s = 0; s < sound_offsets.size(); ++s)
        {
            callbacks.on_progress(std::format("Loading sound {} of {}", s, sound_offsets.size()));
            log_file(activity, file, std::format("Loading sound {} of {}", s, sound_offsets.size()));
            const std::size_t offset = sound_offsets[s];
            const std::size_t size = s == sound_offsets.size() - 1 ?
                sound_data.size() - offset - 1 :
                sound_offsets[s + 1] - offset;
            _sound_samples.push_back(convert_vag_to_wav({ &sound_data[offset], &sound_data[offset + size] }, sample_frequency));
        }

        log_file(activity, file, std::format("Read {} sounds", sound_offsets.size()));
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
    }

    uint16_t attribute_for_object_texture(const tr_object_texture_psx& texture, const tr_clut& clut)
    {
        const uint16_t from_tri = texture.tri_draw & 0x3;
        const uint16_t from_quad = texture.quad_draw & 0x3;
        if (from_tri == 2 || from_quad == 2)
        {
            return 2;
        }
        return std::ranges::any_of(clut.Colour, [](auto&& c) { return c.Red == 0 && c.Green == 0 && c.Blue == 0; }) ? 1 : 0;
    }

    void Level::load_psx_pack(std::basic_ispanstream<uint8_t>& file, trview::Activity&, const LoadCallbacks&)
    {
        if (_pack_source)
        {
            _pack = _pack_source(file);
            _pack->set_filename(_filename);
        }
    }
}
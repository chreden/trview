#include "TileMapper.h"

namespace trlevel
{
    TileMapper::TileMapper(uint32_t initial_textile_count, const std::function<void(const std::vector<uint32_t>&)>& publish_callback)
        : current_textile(256 * 256, 0), textile_number(initial_textile_count), on_publish(publish_callback)
    {
    }

    tr_object_texture TileMapper::map(const std::vector<uint32_t>& data, uint32_t width, uint32_t height)
    {
        find_space(width, height);
        copy_data(data, width, height);

        tr_object_texture new_object_texture
        {
            .Attribute = static_cast<uint16_t>(std::ranges::any_of(data, [](auto&& p) { return (p & 0xff000000) == 0; }) ? 1 : 0),
            .TileAndFlag = static_cast<uint16_t>(textile_number),
            .Vertices =
            {
                { 0, static_cast<uint8_t>(x_current + 1), 0, static_cast<uint8_t>(y_current + 1) },
                { 0, static_cast<uint8_t>(x_current + width - 1), 0, static_cast<uint8_t>(y_current + 1) },
                { 0, static_cast<uint8_t>(x_current + 1), 0, static_cast<uint8_t>(y_current + height - 1) },
                { 0, static_cast<uint8_t>(x_current + width - 1), 0, static_cast<uint8_t>(y_current + height - 1) }
            }
        };

        adjust_cursor(width, height);
        return new_object_texture;
    }

    tr_sprite_texture TileMapper::map_sprite(const std::vector<uint32_t>& data, uint32_t width, uint32_t height, const tr_sprite_texture_saturn& sprite_texture)
    {
        find_space(width, height);
        copy_data(data, width, height);

        tr_sprite_texture new_sprite_texture
        {
            .Tile = static_cast<uint16_t>(textile_number),
            .x = static_cast<uint8_t>(x_current + 1),
            .y = static_cast<uint8_t>(y_current + 1),
            .Width = static_cast<uint16_t>(((width - 2) * 256) + 255),
            .Height = static_cast<uint16_t>(((height - 2) * 256) + 255),
            .LeftSide = sprite_texture.LeftSide,
            .TopSide = sprite_texture.TopSide,
            .RightSide = sprite_texture.RightSide,
            .BottomSide = sprite_texture.BottomSide
        };

        adjust_cursor(width, height);
        return new_sprite_texture;
    }

    void TileMapper::finish()
    {
        publish();
    }

    void TileMapper::find_space(uint32_t width, uint32_t height)
    {
        // Try to tile horizontally first and then move on to the next row when full.
        if (x_current + width > max_x)
        {
            y_current = y_extent;
            x_current = 0;
        }

        if (y_current + height > max_y)
        {
            publish();
        }
    }

    void TileMapper::copy_data(const std::vector<uint32_t>& data, uint32_t width, uint32_t height)
    {
        for (uint32_t y = 0; y < height; ++y)
        {
            memcpy(&current_textile[(y_current + y) * 256 + x_current],
                &data[y * width],
                sizeof(uint32_t) * width);
        }
    }

    void TileMapper::adjust_cursor(uint32_t width, uint32_t height)
    {
        x_current += width;
        y_extent = std::max(y_extent, y_current + height);
    }

    void TileMapper::reset()
    {
        memset(&current_textile[0], 0, sizeof(uint32_t) * current_textile.size());
        x_current = 0u;
        y_current = 0u;
        y_extent = 0u;
    }

    void TileMapper::publish()
    {
        on_publish(current_textile);
        textile_number++;
        reset();
    }
}

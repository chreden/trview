#pragma once

#include "TileMapper.h"
#include "trtypes.h"

#include <functional>
#include <vector>
#include <cstdint>

namespace trlevel
{
    class TileMapper final
    {
    public:
        TileMapper(uint32_t initial_textile_count, const std::function<void(const std::vector<uint32_t>&)>& publish_callback);
        tr_object_texture map(const std::vector<uint32_t>& data, uint32_t width, uint32_t height);
        tr_sprite_texture map_sprite(const std::vector<uint32_t>& data, uint32_t width, uint32_t height, const tr_sprite_texture_saturn& sprite_texture);
        void finish();
    private:
        void find_space(uint32_t width, uint32_t height);
        void copy_data(const std::vector<uint32_t>& data, uint32_t width, uint32_t height);
        void adjust_cursor(uint32_t width, uint32_t height);
        void reset();
        void publish();

        std::vector<uint32_t> current_textile;
        uint32_t              x_current{ 0u };
        uint32_t              y_current{ 0u };
        uint32_t              y_extent{ 0u };
        uint32_t              textile_number{ 0u };
        std::function<void(const std::vector<uint32_t>&)> on_publish;
    };
}

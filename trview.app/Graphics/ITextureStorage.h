#pragma once

#include <string>
#include <cstdint>
#include <trview.graphics/Texture.h>

namespace trview
{
    struct ITextureStorage
    {
        virtual ~ITextureStorage() = 0;
        virtual void add_texture(const std::vector<uint32_t>& pixels, uint32_t width, uint32_t height) = 0;
        virtual graphics::Texture coloured(uint32_t colour) const = 0;
        virtual graphics::Texture geometry_texture() const = 0;
        virtual graphics::Texture lookup(const std::string& key) const = 0;
        virtual uint32_t num_textures() const = 0;
        virtual void store(const std::string& key, const graphics::Texture& texture) = 0;
        virtual graphics::Texture texture(uint32_t tile_index) const = 0;
        virtual graphics::Texture untextured() const = 0;
    };
}

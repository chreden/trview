#pragma once

#include <string>
#include <cstdint>
#include <trview.graphics/Texture.h>

namespace trview
{
    struct ITextureStorage
    {
        virtual ~ITextureStorage() = 0;
        virtual graphics::Texture coloured(uint32_t colour) const = 0;
        virtual graphics::Texture lookup(const std::string& key) const = 0;
        virtual void store(const std::string& key, const graphics::Texture& texture) = 0;
    };
}

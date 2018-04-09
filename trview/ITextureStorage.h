#pragma once

#include <string>
#include <cstdint>
#include <trview.common/Texture.h>

namespace trview
{
    struct ITextureStorage
    {
        virtual ~ITextureStorage() = 0;
        virtual Texture coloured(uint32_t colour) const = 0;
        virtual Texture lookup(const std::string& key) const = 0;
        virtual void store(const std::string& key, const Texture& texture) = 0;
    };
}

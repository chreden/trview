#pragma once

#include <vector>
#include <SimpleMath.h>

#include "ITextureStorage.h"

#include <trlevel/ILevel.h>

namespace trview
{
    struct ILevelTextureStorage : public ITextureStorage
    {
        using Source = std::function<std::shared_ptr<ILevelTextureStorage>(const trlevel::ILevel&)>;

        virtual ~ILevelTextureStorage() = 0;
        virtual graphics::Texture texture(uint32_t texture_index) const = 0;
        virtual graphics::Texture opaque_texture(uint32_t texture_index) const = 0;
        virtual DirectX::SimpleMath::Vector2 uv(uint32_t texture_index, uint32_t uv_index) const = 0;
        virtual uint32_t tile(uint32_t texture_index) const = 0;
        virtual uint32_t num_tiles() const = 0;
        virtual uint16_t attribute(uint32_t texture_index) const = 0;
        virtual DirectX::SimpleMath::Color palette_from_texture(uint32_t texture) const = 0;
        virtual uint32_t num_object_textures() const = 0;
        virtual trlevel::PlatformAndVersion platform_and_version() const = 0;
        virtual void update(float delta) = 0;
        virtual bool is_animated(uint32_t texture_index) const = 0;
        virtual std::vector<uint32_t> animated_texture(uint32_t texture_index) const = 0;
    };
}


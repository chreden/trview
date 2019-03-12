#pragma once

#include <trview.app/Graphics/ITextureStorage.h>
#include <unordered_map>
#include <trview.graphics/Device.h>

namespace trview
{
    class TextureStorage final : public ITextureStorage
    {
    public:
        explicit TextureStorage(const graphics::Device& device);
        virtual ~TextureStorage() = default;
        virtual graphics::Texture coloured(uint32_t colour) const override;
        virtual graphics::Texture lookup(const std::string& key) const override;
        virtual void store(const std::string& key, const graphics::Texture& texture) override;
    private:
        const graphics::Device& _device;
        std::unordered_map<std::string, graphics::Texture> _textures;
    };
}

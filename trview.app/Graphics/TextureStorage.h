#pragma once

#include <trview.app/Graphics/ITextureStorage.h>
#include <unordered_map>
#include <trview.graphics/IDevice.h>

namespace trview
{
    class TextureStorage final : public ITextureStorage
    {
    public:
        explicit TextureStorage(const std::shared_ptr<graphics::IDevice>& device);
        virtual ~TextureStorage() = default;
        void add_texture(const std::vector<uint32_t>& pixels, uint32_t width, uint32_t height) override;
        graphics::Texture coloured(uint32_t colour) const override;
        graphics::Texture geometry_texture() const override;
        graphics::Texture lookup(const std::string& key) const override;
        uint32_t num_textures() const override;
        void store(const std::string& key, const graphics::Texture& texture) override;
        graphics::Texture texture(uint32_t tile_index) const override;
        graphics::Texture untextured() const override;
    private:
        std::shared_ptr<graphics::IDevice> _device;
        std::unordered_map<std::string, graphics::Texture> _textures;
        graphics::Texture _geometry_texture;
        std::vector<graphics::Texture> _tiles;
        mutable graphics::Texture _untextured_texture;
    };
}

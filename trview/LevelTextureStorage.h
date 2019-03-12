#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include <vector>
#include <memory>
#include <SimpleMath.h>

#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>

#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.graphics/Device.h>

namespace trview
{
    class LevelTextureStorage final : public ILevelTextureStorage
    {
    public:
        explicit LevelTextureStorage(const graphics::Device& device, const trlevel::ILevel& level);
        virtual ~LevelTextureStorage() = default;
        virtual graphics::Texture texture(uint32_t tile_index) const override;
        virtual graphics::Texture coloured(uint32_t colour) const override;
        virtual graphics::Texture lookup(const std::string& key) const override;
        virtual void              store(const std::string& key, const graphics::Texture& texture) override;
        virtual graphics::Texture untextured() const override;
        virtual DirectX::SimpleMath::Vector2 uv(uint32_t texture_index, uint32_t uv_index) const override;
        virtual uint32_t          tile(uint32_t texture_index) const override;
        virtual uint32_t          num_tiles() const override;
        virtual uint16_t attribute(uint32_t texture_index) const override;
        virtual DirectX::SimpleMath::Color palette_from_texture(uint32_t texture) const override;
    private:
        const graphics::Device& _device;
        std::vector<graphics::Texture> _tiles;
        std::vector<trlevel::tr_object_texture> _object_textures;
        std::unique_ptr<ITextureStorage> _texture_storage;
        mutable graphics::Texture _untextured_texture;
        const trlevel::ILevel& _level;
    };
}

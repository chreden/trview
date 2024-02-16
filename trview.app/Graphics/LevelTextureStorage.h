#pragma once

#include <vector>
#include <array>
#include <memory>
#include <SimpleMath.h>
#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>
#include "ILevelTextureStorage.h"
#include <trview.graphics/IDevice.h>
#include <trview.common/IFiles.h>

namespace trview
{
    class LevelTextureStorage final : public ILevelTextureStorage
    {
    public:
        explicit LevelTextureStorage(const std::shared_ptr<graphics::IDevice>& device, std::unique_ptr<ITextureStorage> texture_storage, const std::shared_ptr<trlevel::ILevel>& level, const std::shared_ptr<IFiles>& files);
        virtual ~LevelTextureStorage() = default;
        virtual graphics::Texture texture(uint32_t tile_index) const override;
        virtual graphics::Texture opaque_texture(uint32_t texture_index) const override;
        virtual graphics::Texture coloured(uint32_t colour) const override;
        virtual graphics::Texture lookup(const std::string& key) const override;
        virtual void              store(const std::string& key, const graphics::Texture& texture) override;
        virtual graphics::Texture untextured() const override;
        virtual DirectX::SimpleMath::Vector2 uv(uint32_t texture_index, uint32_t uv_index) const override;
        virtual uint32_t          tile(uint32_t texture_index) const override;
        virtual uint32_t          num_tiles() const override;
        virtual uint16_t attribute(uint32_t texture_index) const override;
        virtual DirectX::SimpleMath::Color palette_from_texture(uint32_t texture) const override;
        virtual graphics::Texture geometry_texture() const override;
        virtual uint32_t num_object_textures() const override;
        graphics::Texture remastered_texture(uint32_t texture_index) const override;
        uint32_t num_remastered_textures() const override;
    private:
        void determine_texture_mode();
        void generate_remastered_textures(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<trlevel::ILevel>& level);

        std::weak_ptr<trlevel::ILevel> _level;

        std::vector<graphics::Texture> _tiles;
        std::vector<graphics::Texture> _opaque_tiles;
        std::vector<trlevel::tr_object_texture> _object_textures;
        std::unique_ptr<ITextureStorage> _texture_storage;
        mutable graphics::Texture _untextured_texture;
        std::array<DirectX::SimpleMath::Color, 256> _palette;
        trlevel::LevelVersion _version;
        trlevel::Platform _platform;

        enum class TextureMode
        {
            Official,
            Custom
        };
        TextureMode _texture_mode{ TextureMode::Official };
        graphics::Texture _geometry_texture;

        std::vector<graphics::Texture> _remastered_textures;
    };
}

#pragma once

#include <vector>
#include <array>
#include <memory>
#include <SimpleMath.h>
#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>
#include <trview.app/Graphics/ILevelTextureStorage.h>
#include <trview.graphics/IDevice.h>

namespace trview
{
    class LevelTextureStorage final : public ILevelTextureStorage
    {
    public:
        explicit LevelTextureStorage(const std::shared_ptr<graphics::IDevice>& device, std::unique_ptr<ITextureStorage> texture_storage);
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
        void load(const std::shared_ptr<trlevel::ILevel>& level);
        void add_textile(const std::vector<uint32_t>& textile);
    private:
        void determine_texture_mode();

        std::weak_ptr<trlevel::ILevel> _level;

        std::shared_ptr<graphics::IDevice> _device;
        std::vector<graphics::Texture> _tiles;
        std::vector<graphics::Texture> _opaque_tiles;
        std::vector<trlevel::tr_object_texture> _object_textures;
        std::unique_ptr<ITextureStorage> _texture_storage;
        mutable graphics::Texture _untextured_texture;
        std::array<DirectX::SimpleMath::Color, 256> _palette;
        trlevel::LevelVersion _version{ trlevel::LevelVersion::Unknown };
        trlevel::Platform _platform{ trlevel::Platform::Unknown };

        enum class TextureMode
        {
            Official,
            Custom
        };
        TextureMode _texture_mode{ TextureMode::Official };
        graphics::Texture _geometry_texture;
    };
}

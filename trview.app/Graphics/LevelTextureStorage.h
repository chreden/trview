#pragma once

#include <vector>
#include <array>
#include <memory>
#include <unordered_map>
#include <unordered_set>
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
        void add_texture(const std::vector<uint32_t>& pixels, uint32_t width, uint32_t height) override;
        virtual graphics::Texture texture(uint32_t tile_index) const override;
        virtual graphics::Texture opaque_texture(uint32_t texture_index) const override;
        virtual graphics::Texture coloured(uint32_t colour) const override;
        virtual graphics::Texture lookup(const std::string& key) const override;
        virtual void              store(const std::string& key, const graphics::Texture& texture) override;
        virtual graphics::Texture untextured() const override;
        virtual DirectX::SimpleMath::Vector2 uv(uint32_t texture_index, uint32_t uv_index) const override;
        virtual uint32_t          tile(uint32_t texture_index) const override;
        uint32_t num_textures() const override; 
        virtual uint32_t          num_tiles() const override;
        virtual uint16_t attribute(uint32_t texture_index) const override;
        virtual DirectX::SimpleMath::Color palette_from_texture(uint32_t texture) const override;
        virtual graphics::Texture geometry_texture() const override;
        virtual uint32_t num_object_textures() const override;
        trlevel::PlatformAndVersion platform_and_version() const override;
        void load(const std::shared_ptr<trlevel::ILevel>& level);
        void add_textile(const std::vector<uint32_t>& textile, uint32_t width, uint32_t height);
        Triangle::AnimationMode animation_mode(uint32_t texture_index) const override;
        std::vector<uint32_t> animated_texture(uint32_t texture_index) const override;
    private:
        void determine_texture_mode();
        void generate_replacement_textures();

        std::weak_ptr<trlevel::ILevel> _level;
        std::shared_ptr<graphics::IDevice> _device;
        std::vector<graphics::Texture> _opaque_tiles;
        std::vector<trlevel::tr_object_texture> _object_textures;
        std::unique_ptr<ITextureStorage> _texture_storage;
        std::array<DirectX::SimpleMath::Color, 256> _palette;
        trlevel::PlatformAndVersion _platform_and_version;
        enum class TextureMode
        {
            Official,
            Custom
        };
        TextureMode _texture_mode{ TextureMode::Official };
        std::unordered_map<uint32_t, std::vector<uint32_t>> _animated_textures;

        struct UVTextureReplacment
        {
            trlevel::tr_object_texture object_texture;
            uint32_t new_tile;
        };
        std::unordered_map<uint32_t, UVTextureReplacment> _animated_uv_textures;

        // Texture replacement mode:
        struct SourceTexture
        {
            uint32_t width;
            uint32_t height;
            std::vector<uint32_t> bytes;
        };

        struct TextureReplacement
        {
            std::vector<DirectX::SimpleMath::Vector2> uvs;
            uint32_t tile;
        };
        std::vector<SourceTexture> _source_textures;
        std::unordered_map<uint32_t, TextureReplacement> _texture_replacements;
    };
}

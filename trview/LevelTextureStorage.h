#pragma once

#include <atlbase.h>
#include <d3d11.h>
#include <vector>
#include <memory>

#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>

#include "ILevelTextureStorage.h"

namespace trview
{
    class LevelTextureStorage final : public ILevelTextureStorage
    {
    public:
        explicit LevelTextureStorage(CComPtr<ID3D11Device> device, const trlevel::ILevel& level);
        virtual ~LevelTextureStorage() = default;
        virtual Texture           texture(uint32_t tile_index) const override;
        virtual Texture           coloured(uint32_t colour) const override;
        virtual Texture           lookup(const std::string& key) const override;
        virtual void              store(const std::string& key, const Texture& texture) override;
        virtual Texture           untextured() const override;
        virtual DirectX::XMFLOAT2 uv(uint32_t texture_index, uint32_t uv_index) const override;
        virtual uint32_t          tile(uint32_t texture_index) const override;
        virtual uint32_t          num_tiles() const override;
        virtual DirectX::XMFLOAT4 palette_from_texture(uint32_t texture) const override;
        virtual DirectX::XMFLOAT4 palette(uint32_t index) const override;
    private:
        CComPtr<ID3D11Device> _device;
        std::vector<Texture>  _tiles;
        std::vector<trlevel::tr_object_texture> _object_textures;
        std::unique_ptr<ITextureStorage> _texture_storage;
        mutable Texture _untextured_texture;
        const trlevel::ILevel& _level;
    };
}

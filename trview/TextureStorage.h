#pragma once

#include <atlbase.h>
#include <d3d11.h>
#include <vector>

#include <trlevel/trtypes.h>
#include <trlevel/ILevel.h>

#include "ITextureStorage.h"

namespace trview
{
    class TextureStorage final : public ITextureStorage
    {
    public:
        explicit TextureStorage(CComPtr<ID3D11Device> device, const trlevel::ILevel& level);
        virtual ~TextureStorage() = default;
        virtual Texture           texture(uint32_t tile_index) const override;
        virtual Texture           untextured() const override;
        virtual DirectX::XMFLOAT2 uv(uint32_t texture_index, uint32_t uv_index) const override;
        virtual uint32_t          tile(uint32_t texture_index) const override;
        virtual uint32_t          num_tiles() const override;
        virtual DirectX::XMFLOAT4 palette_from_texture(uint32_t texture) const override;
        virtual DirectX::XMFLOAT4 palette(uint32_t index) const override;
    private:
        std::vector<Texture> _tiles;
        std::vector<trlevel::tr_object_texture> _object_textures;
        Texture _untextured_texture;
        const trlevel::ILevel& _level;
    };
}

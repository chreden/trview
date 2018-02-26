#include "stdafx.h"
#include "TextureStorage.h"

namespace trview
{
    TextureStorage::TextureStorage(CComPtr<ID3D11Device> device, const trlevel::ILevel& level)
        : _level(level)
    {
        // Load the textures from the level and then allow to cycle through them?
        for (uint32_t i = 0; i < level.num_textiles(); ++i)
        {
            auto t16 = level.get_textile16(i);

            std::vector<uint32_t> data(256 * 256, 0u);

            uint32_t index = 0;
            for (auto t : t16.Tile)
            {
                data[index++] = trlevel::convert_textile16(t);
            }

            D3D11_SUBRESOURCE_DATA srd;
            memset(&srd, 0, sizeof(srd));
            srd.pSysMem = &data[0];
            srd.SysMemPitch = sizeof(uint32_t) * 256;

            D3D11_TEXTURE2D_DESC desc;
            memset(&desc, 0, sizeof(desc));
            desc.Width = 256;
            desc.Height = 256;
            desc.MipLevels = desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;

            Texture tex;
            device->CreateTexture2D(&desc, &srd, &tex.texture);
            device->CreateShaderResourceView(tex.texture, nullptr, &tex.view);
            _tiles.push_back(tex);
        }

        // Copy object textures locally from the level.
        for (uint32_t i = 0; i < level.num_object_textures(); ++i)
        {
            _object_textures.push_back(level.get_object_texture(i));
        }

        uint32_t pixel = 0xffffffff;
        D3D11_SUBRESOURCE_DATA srd;
        memset(&srd, 0, sizeof(srd));
        srd.pSysMem = &pixel;
        srd.SysMemPitch = sizeof(uint32_t);

        D3D11_TEXTURE2D_DESC tex_desc;
        memset(&tex_desc, 0, sizeof(tex_desc));
        tex_desc.Width = 1;
        tex_desc.Height = 1;
        tex_desc.MipLevels = tex_desc.ArraySize = 1;
        tex_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        tex_desc.SampleDesc.Count = 1;
        tex_desc.Usage = D3D11_USAGE_DEFAULT;
        tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        tex_desc.CPUAccessFlags = 0;
        tex_desc.MiscFlags = 0;

        device->CreateTexture2D(&tex_desc, &srd, &_untextured_texture.texture);
        device->CreateShaderResourceView(_untextured_texture.texture, nullptr, &_untextured_texture.view);
    }

    Texture TextureStorage::texture(uint32_t tile_index) const
    {
        return _tiles[tile_index];
    }

    Texture TextureStorage::untextured() const
    {
        return _untextured_texture;
    }

    DirectX::XMFLOAT2 TextureStorage::uv(uint32_t texture_index, uint32_t uv_index) const
    {
        const auto& vert = _object_textures[texture_index].Vertices[uv_index];
        return DirectX::XMFLOAT2(vert.Xpixel / 255.0f, vert.Ypixel / 255.0f);
    }

    uint32_t TextureStorage::tile(uint32_t texture_index) const
    {
        return _object_textures[texture_index].TileAndFlag & 0x7FFF;
    }

    uint32_t TextureStorage::num_tiles() const
    {
        return _tiles.size();
    }

    DirectX::XMFLOAT4 TextureStorage::palette_from_texture(uint32_t texture) const
    {
        return palette((texture & 0x7fff) >> 8);
    }

    DirectX::XMFLOAT4 TextureStorage::palette(uint32_t index) const
    {
        auto palette = _level.get_palette_entry_16(index);
        return DirectX::XMFLOAT4(palette.Red / 255.f, palette.Green / 255.f, palette.Blue / 255.f, 1.0f);
    }
}
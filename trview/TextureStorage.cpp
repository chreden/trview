#include "stdafx.h"
#include "TextureStorage.h"

namespace trview
{
    TextureStorage::TextureStorage(const Microsoft::WRL::ComPtr<ID3D11Device>& device)
        : _device(device)
    {
    }

    Texture TextureStorage::coloured(uint32_t colour) const
    {
        D3D11_SUBRESOURCE_DATA srd;
        memset(&srd, 0, sizeof(srd));
        srd.pSysMem = &colour;
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

        Texture texture;
        _device->CreateTexture2D(&tex_desc, &srd, &texture.texture);
        _device->CreateShaderResourceView(texture.texture.Get(), nullptr, &texture.view);
        return texture;
    }

    Texture TextureStorage::lookup(const std::string& key) const
    {
        auto found = _textures.find(key);
        if (found == _textures.end())
        {
            return Texture();
        }
        return found->second;
    }

    void TextureStorage::store(const std::string& key, const Texture& texture)
    {
        _textures.insert({ key, texture });
    }
}

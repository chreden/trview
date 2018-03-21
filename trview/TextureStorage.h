#pragma once

#include "ITextureStorage.h"
#include <unordered_map>

namespace trview
{
    class TextureStorage final : public ITextureStorage
    {
    public:
        explicit TextureStorage(CComPtr<ID3D11Device> device);
        virtual ~TextureStorage() = default;
        virtual Texture coloured(uint32_t colour) const override;
        virtual Texture lookup(const std::string& key) const override;
        virtual void store(const std::string& key, const Texture& texture) override;
    private:
        CComPtr<ID3D11Device> _device;
        std::unordered_map<std::string, Texture> _textures;
    };
}

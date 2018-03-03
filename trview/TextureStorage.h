#pragma once

#include "ITextureStorage.h"

namespace trview
{
    class TextureStorage : public ITextureStorage
    {
    public:
        explicit TextureStorage(CComPtr<ID3D11Device> device);
        virtual ~TextureStorage();
        virtual Texture coloured(uint32_t colour) const override;
    private:
        CComPtr<ID3D11Device> _device;
    };
}

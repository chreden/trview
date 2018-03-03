#pragma once

#include "ITextureStorage.h"

namespace trview
{
    class TextureStorage : public ITextureStorage
    {
    public:
        explicit TextureStorage(CComPtr<ID3D11Device> device);
        virtual ~TextureStorage();
        virtual Texture untextured() const override;
    private:
        Texture _untextured;
    };
}

#pragma once

#include <atlbase.h>
#include <d3d11.h>

#include <vector>
#include <cstdint>

#include "IShader.h"

namespace trview
{
    class PixelShader final : public IShader
    {
    public:
        PixelShader(const CComPtr<ID3D11Device>& device, const std::vector<uint8_t>& data);

        virtual ~PixelShader() = default;

        virtual void PixelShader::apply(const CComPtr<ID3D11DeviceContext>& context) override;
    private:
        CComPtr<ID3D11PixelShader> _pixel_shader;
    };
}


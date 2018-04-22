#include "PixelShader.h"

namespace trview
{
    namespace graphics
    {
        PixelShader::PixelShader(const CComPtr<ID3D11Device>& device, const std::vector<uint8_t>& data)
        {
            if (data.empty())
            {
                throw std::exception("Data for PixelShader cannot be empty");
            }

            device->CreatePixelShader(&data[0], data.size(), nullptr, &_pixel_shader);
        }

        void PixelShader::apply(const CComPtr<ID3D11DeviceContext>& context)
        {
            context->PSSetShader(_pixel_shader, nullptr, 0);
        }
    }
}

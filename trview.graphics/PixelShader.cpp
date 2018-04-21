#include "PixelShader.h"

namespace trview
{
    namespace graphics
    {
        PixelShader::PixelShader(const CComPtr<ID3D11Device>& device, const std::vector<uint8_t>& data)
        {
            // Check for zero length data (throw exception).
            HRESULT result = device->CreatePixelShader(&data[0], data.size(), nullptr, &_pixel_shader);
            // Do more things with the HRESULT here - throw errors if there were any as we need the shaders...
        }

        void PixelShader::apply(const CComPtr<ID3D11DeviceContext>& context)
        {
            context->PSSetShader(_pixel_shader, nullptr, 0);
        }
    }
}

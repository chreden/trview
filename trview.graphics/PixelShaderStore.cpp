#include "PixelShaderStore.h"

using namespace Microsoft::WRL;

namespace trview
{
    namespace graphics
    {
        PixelShaderStore::PixelShaderStore(const ComPtr<ID3D11DeviceContext>& context)
            : _context(context)
        {
            _context->PSGetShader(&_pixel_shader, nullptr, nullptr);
        }

        PixelShaderStore::~PixelShaderStore()
        {
            _context->PSSetShader(_pixel_shader.Get(), nullptr, 0);
        }
    }
}
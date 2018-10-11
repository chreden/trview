#pragma once

#include <wrl/client.h>
#include <d3d11.h>

namespace trview
{
    namespace graphics
    {
        class PixelShaderStore final
        {
        public:
            PixelShaderStore(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
            ~PixelShaderStore();
        private:
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
            Microsoft::WRL::ComPtr<ID3D11PixelShader>  _pixel_shader;
        };
    }
}
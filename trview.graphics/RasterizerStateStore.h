#pragma once

#include <wrl/client.h>
#include <d3d11.h>

namespace trview
{
    namespace graphics
    {
        class RasterizerStateStore final
        {
        public:
            explicit RasterizerStateStore(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
            ~RasterizerStateStore();
        private:
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
            Microsoft::WRL::ComPtr<ID3D11RasterizerState> _rasterizer;
        };
    }
}

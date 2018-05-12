#pragma once

#include <wrl/client.h>
#include <d3d11.h>

namespace trview
{
    namespace graphics
    {
        class RenderTargetStore final
        {
        public:
            explicit RenderTargetStore(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& context);
            ~RenderTargetStore();
        private:
            Microsoft::WRL::ComPtr<ID3D11DeviceContext> _context;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> _render_target;
        };
    }
}
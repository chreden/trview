#pragma once

#include <atlbase.h>
#include <d3d11.h>

namespace trview
{
    namespace ui
    {
        namespace render
        {
            class RenderTargetStore
            {
            public:
                explicit RenderTargetStore(CComPtr<ID3D11DeviceContext> context);
                ~RenderTargetStore();
            private:
                CComPtr<ID3D11DeviceContext> _context;
                CComPtr<ID3D11RenderTargetView> _render_target;
            };
        }
    }
}
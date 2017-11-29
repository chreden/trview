#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <atlbase.h>

namespace trview
{
    class Viewer
    {
    public:
        explicit Viewer(HWND window);

        void render();
    private:
        CComPtr<IDXGISwapChain>         _swap_chain;
        CComPtr<ID3D11Device>           _device;
        CComPtr<ID3D11DeviceContext>    _context;
        CComPtr<ID3D11RenderTargetView> _render_target_view;
    };
}

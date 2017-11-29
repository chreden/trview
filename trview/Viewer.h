#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <atlbase.h>
#include <string>

namespace trview
{
    class Viewer
    {
    public:
        explicit Viewer(HWND window);

        void render();

        void open(const std::wstring filename);
    private:
        void initialise_d3d(HWND window);

        CComPtr<IDXGISwapChain>         _swap_chain;
        CComPtr<ID3D11Device>           _device;
        CComPtr<ID3D11DeviceContext>    _context;
        CComPtr<ID3D11RenderTargetView> _render_target_view;
    };
}

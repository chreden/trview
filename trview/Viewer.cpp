#include "stdafx.h"
#include "Viewer.h"
#include "..\trlevel\trlevel.h"

namespace trview
{
    Viewer::Viewer(HWND window)
    {
        initialise_d3d(window);


    }

    void Viewer::initialise_d3d(HWND window)
    {
        RECT window_rectangle;
        GetClientRect(window, &window_rectangle);

        // Swap chain description.
        DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
        swap_chain_desc.BufferCount = 1;
        swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_desc.BufferDesc.Height = window_rectangle.bottom;
        swap_chain_desc.BufferDesc.Width = window_rectangle.right;
        swap_chain_desc.BufferDesc.RefreshRate.Numerator = 1;
        swap_chain_desc.BufferDesc.RefreshRate.Denominator = 60;
        swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swap_chain_desc.OutputWindow = window;
        swap_chain_desc.Windowed = TRUE;
        swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        swap_chain_desc.SampleDesc.Count = 1;
        swap_chain_desc.SampleDesc.Quality = 0;

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &swap_chain_desc,
            &_swap_chain,
            &_device,
            nullptr,
            &_context);

        CComPtr<ID3D11Texture2D> back_buffer;

        _swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&back_buffer));
        _device->CreateRenderTargetView(back_buffer, nullptr, &_render_target_view);

        D3D11_VIEWPORT viewport;
        viewport.Width = static_cast<float>(window_rectangle.right);
        viewport.Height = static_cast<float>(window_rectangle.bottom);
        viewport.MaxDepth = 1;
        viewport.MinDepth = 0;
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        _context->RSSetViewports(1, &viewport);
    }

    void Viewer::open(const std::wstring filename)
    {
        auto level = trlevel::load_level(filename);
    }

    void Viewer::render()
    {
        _context->OMSetRenderTargets(1, &_render_target_view.p, nullptr);

        float colours[4] = { 1.f, 0.f, 0.f, 1.f };
        _context->ClearRenderTargetView(_render_target_view, colours);

        _swap_chain->Present(1, 0);
    }
}

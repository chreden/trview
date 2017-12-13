#include "stdafx.h"
#include "Viewer.h"
#include "..\trlevel\trlevel.h"

#include <vector>
#include <sstream>
#include <string>
#include <fstream>

#include <wincodec.h>
#include <d3dcompiler.h>

namespace trview
{
    Viewer::Viewer(HWND window)
    {
        initialise_d3d(window);

        RECT client_window;
        GetClientRect(window, &client_window);
        _texture_window = std::make_unique<TextureWindow>(_device, client_window.right - client_window.left, client_window.bottom - client_window.top);
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
            D3D11_CREATE_DEVICE_BGRA_SUPPORT,
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

        D3D11_BLEND_DESC desc;
        memset(&desc, 0, sizeof(desc));
        desc.RenderTarget[0].BlendEnable = true;
        desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
        desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        _device->CreateBlendState(&desc, &_blend_state);
    }

    void Viewer::open(const std::wstring filename)
    {
        _level_textures.clear();
        _current_level = trlevel::load_level(filename);

        // Load the textures from the level and then allow to cycle through them?
        for (uint32_t i = 0; i < _current_level->num_textiles(); ++i)
        {
            auto t16 = _current_level->get_textile16(i);

            std::vector<uint32_t> data;
            data.resize(256 * 256, 0);

            uint32_t index = 0;
            for (auto t : t16.Tile)
            {
                data[index++] = trlevel::convert_textile16(t);
            }

            D3D11_SUBRESOURCE_DATA srd;
            memset(&srd, 0, sizeof(srd));
            srd.pSysMem = &data[0];
            srd.SysMemPitch = sizeof(uint32_t) * 256;

            D3D11_TEXTURE2D_DESC desc;
            memset(&desc, 0, sizeof(desc));
            desc.Width = 256;
            desc.Height = 256;
            desc.MipLevels = desc.ArraySize = 1;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.SampleDesc.Count = 1;
            desc.Usage = D3D11_USAGE_DYNAMIC;
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
            desc.MiscFlags = 0;

            Texture tex;
            _device->CreateTexture2D(&desc, &srd, &tex.texture);
            _device->CreateShaderResourceView(tex.texture, nullptr, &tex.view);
            _level_textures.push_back(tex);
        }

        // Set the textures in the viewer.
        std::vector<CComPtr<ID3D11ShaderResourceView>> texture_views;
        for (auto& t : _level_textures)
        {
            texture_views.push_back(t.view);
        }
        _texture_window->set_textures(texture_views);
    }

    void Viewer::render()
    {
        _context->OMSetRenderTargets(1, &_render_target_view.p, nullptr);
        _context->OMSetBlendState(_blend_state, 0, 0xffffffff);

        float colours[4] = { 0.f, 0.2f, 0.4f, 1.f };
        _context->ClearRenderTargetView(_render_target_view, colours);

        _texture_window->render(_context);

        _swap_chain->Present(1, 0);
    }

    void Viewer::cycle()
    {
        _texture_window->cycle();
    }
}

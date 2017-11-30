#include "stdafx.h"
#include "Viewer.h"
#include "..\trlevel\trlevel.h"

#include <vector>

#include "..\DirectXTex\DirectXTex-master\DirectXTex\DirectXTex.h"
#include <wincodec.h>

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
                uint16_t r = t & 0x001f;
                uint16_t g = (t & 0x03e0) >> 5;
                uint16_t b = (t & 0x7c00) >> 10;

                r = static_cast<uint16_t>(static_cast<double>(r) / 0x1f * 0xff);
                g = static_cast<uint16_t>(static_cast<double>(g) / 0x1f * 0xff);
                b = static_cast<uint16_t>(static_cast<double>(b) / 0x1f * 0xff);

                data[index++] = 0xff << 24 | b | g << 8 | r << 16;
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

            CComPtr<ID3D11Texture2D> texture;
            _device->CreateTexture2D(&desc, &srd, &texture);

            DirectX::ScratchImage image;
            DirectX::CaptureTexture(_device, _context, texture, image);
            auto im = image.GetImage(0, 0, 0);
            HRESULT hr = DirectX::SaveToWICFile(im, 1, 0, GUID_ContainerFormatPng, L"new_image.png");
        }
    }

    void Viewer::render()
    {
        _context->OMSetRenderTargets(1, &_render_target_view.p, nullptr);

        float colours[4] = { 1.f, 0.f, 0.f, 1.f };
        _context->ClearRenderTargetView(_render_target_view, colours);

        _swap_chain->Present(1, 0);
    }
}

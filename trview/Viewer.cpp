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

        _font_factory = std::make_unique<FontFactory>();

        RECT client_window;
        GetClientRect(window, &client_window);

        uint32_t width = client_window.right - client_window.left;
        uint32_t height = client_window.bottom - client_window.top;

        _texture_window = std::make_unique<TextureWindow>(_device, *_font_factory, width, height);
        _room_window = std::make_unique<RoomWindow>(_device, *_font_factory, width, height);
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

    void Viewer::generate_textures()
    {
        _level_textures.clear();

        // Load the textures from the level and then allow to cycle through them?
        for (uint32_t i = 0; i < _current_level->num_textiles(); ++i)
        {
            auto t16 = _current_level->get_textile16(i);

            std::vector<uint32_t> data(256 * 256, 0u);

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
    }

    void Viewer::generate_rooms()
    {
        const uint16_t num_rooms = _current_level->num_rooms();
        for (uint16_t i = 0; i < num_rooms; ++i)
        {
            auto room = _current_level->get_room(i);

            Room new_room(room.info.x, room.info.z, room.info.yBottom, room.info.yTop);

            // Convert that room into a Room that we can use in the UI.
            _level_rooms.push_back(new_room);
        }
    }

    void Viewer::open(const std::wstring filename)
    {
        _level_textures.clear();
        _level_rooms.clear();
        _current_level = trlevel::load_level(filename);

        generate_textures();

        generate_rooms();

        // Set up the views.
        _texture_window->set_textures(_level_textures);

        _room_window->set_rooms(_level_rooms);
    }

    void Viewer::render()
    {
        _context->OMSetRenderTargets(1, &_render_target_view.p, nullptr);
        _context->OMSetBlendState(_blend_state, 0, 0xffffffff);

        float colours[4] = { 0.f, 0.2f, 0.4f, 1.f };
        _context->ClearRenderTargetView(_render_target_view, colours);

        _texture_window->render(_context);
        _room_window->render(_context);

        _swap_chain->Present(1, 0);
    }

    void Viewer::cycle()
    {
        _texture_window->cycle();
    }

    void Viewer::cycle_room()
    {
        _room_window->cycle();
    }

    void Viewer::toggle_room_window()
    {
        _room_window->toggle_visibility();
    }

    void Viewer::toggle_texture_window()
    {
        _texture_window->toggle_visibility();
    }
}

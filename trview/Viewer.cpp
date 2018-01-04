#include "stdafx.h"
#include "Viewer.h"
#include "..\trlevel\trlevel.h"

#include "FileLoader.h"

#include <vector>
#include <sstream>
#include <string>
#include <fstream>

#include <wincodec.h>
#include <d3dcompiler.h>

#include <directxmath.h>
#include <array>

namespace trview
{
    Viewer::Viewer(HWND window)
        : _window(window)
    {
        recalculate_size();

        initialise_d3d(window);

        _font_factory = std::make_unique<FontFactory>();

        _texture_window = std::make_unique<TextureWindow>(_device, *_font_factory, _width, _height);
        _room_window = std::make_unique<RoomWindow>(_device, *_font_factory, _width, _height);
    }

    void Viewer::initialise_d3d(HWND window)
    {
        // Swap chain description.
        DXGI_SWAP_CHAIN_DESC swap_chain_desc{};
        swap_chain_desc.BufferCount = 1;
        swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        swap_chain_desc.BufferDesc.Height = _height;
        swap_chain_desc.BufferDesc.Width = _width;
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
        viewport.Width = static_cast<float>(_width);
        viewport.Height = static_cast<float>(_height);
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

        // Initialize the description of the depth buffer.
        D3D11_TEXTURE2D_DESC depthBufferDesc;
        memset(&depthBufferDesc, 0, sizeof(depthBufferDesc));

        // Set up the description of the depth buffer.
        depthBufferDesc.Width = _width;
        depthBufferDesc.Height = _height;
        depthBufferDesc.MipLevels = 1;
        depthBufferDesc.ArraySize = 1;
        depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthBufferDesc.SampleDesc.Count = 1;
        depthBufferDesc.SampleDesc.Quality = 0;
        depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        depthBufferDesc.CPUAccessFlags = 0;
        depthBufferDesc.MiscFlags = 0;

        _device->CreateTexture2D(&depthBufferDesc, NULL, &_depth_stencil_buffer);

        // Initialize the description of the stencil state.
        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        memset(&depthStencilDesc, 0, sizeof(depthStencilDesc));

        // Set up the description of the stencil state.
        depthStencilDesc.DepthEnable = true;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

        depthStencilDesc.StencilEnable = true;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;

        // Stencil operations if pixel is front-facing.
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil operations if pixel is back-facing.
        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        _device->CreateDepthStencilState(&depthStencilDesc, &_depth_stencil_state);

        // Initialize the description of the stencil state.
        D3D11_DEPTH_STENCIL_DESC ui_depth_stencil_desc;
        memset(&ui_depth_stencil_desc, 0, sizeof(ui_depth_stencil_desc));

        // Set up the description of the stencil state.
        depthStencilDesc.DepthEnable = false;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

        depthStencilDesc.StencilEnable = false;
        depthStencilDesc.StencilReadMask = 0xFF;
        depthStencilDesc.StencilWriteMask = 0xFF;

        // Stencil operations if pixel is front-facing.
        depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // Stencil operations if pixel is back-facing.
        depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        _device->CreateDepthStencilState(&ui_depth_stencil_desc, &_ui_depth_stencil_state);

        _context->OMSetDepthStencilState(_depth_stencil_state, 1);

        D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
        memset(&depthStencilViewDesc, 0, sizeof(depthStencilViewDesc));

        // Set up the depth stencil view description.
        depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        depthStencilViewDesc.Texture2D.MipSlice = 0;

        // Create the depth stencil view.
        _device->CreateDepthStencilView(_depth_stencil_buffer, &depthStencilViewDesc, &_depth_stencil_view);

        std::vector<char> vs_data = load_file(L"level_vertex_shader.cso");

        D3D11_INPUT_ELEMENT_DESC input_desc[3];
        memset(&input_desc, 0, sizeof(input_desc));
        input_desc[0].SemanticName = "Position";
        input_desc[0].SemanticIndex = 0;
        input_desc[0].InstanceDataStepRate = 0;
        input_desc[0].InputSlot = 0;
        input_desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;

        input_desc[1].SemanticName = "Texcoord";
        input_desc[1].SemanticIndex = 0;
        input_desc[1].InstanceDataStepRate = 0;
        input_desc[1].InputSlot = 0;
        input_desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        input_desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;

        input_desc[2].SemanticName = "Texcoord";
        input_desc[2].SemanticIndex = 1;
        input_desc[2].InstanceDataStepRate = 0;
        input_desc[2].InputSlot = 0;
        input_desc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        input_desc[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
        input_desc[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

        hr = _device->CreateInputLayout(input_desc, 3, &vs_data[0], vs_data.size(), &_input_layout);

        hr = _device->CreateVertexShader(&vs_data[0], vs_data.size(), nullptr, &_vertex_shader);

        std::vector<char> ps_data = load_file(L"level_pixel_shader.cso");
        hr = _device->CreatePixelShader(&ps_data[0], ps_data.size(), nullptr, &_pixel_shader);

        // Create a texture sampler state description.
        D3D11_SAMPLER_DESC sampler_desc;
        memset(&desc, 0, sizeof(sampler_desc));
        sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampler_desc.MaxAnisotropy = 1;
        sampler_desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

        // Create the texture sampler state.
        _device->CreateSamplerState(&sampler_desc, &_sampler_state);
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

            // Convert that room into a Room that we can use in the UI.
            _level_rooms.push_back(std::make_unique<Room>(_device, *_current_level, room));
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

        std::vector<RoomInfo> room_infos;
        for (const auto& r : _level_rooms)
        {
            room_infos.push_back(r->info());
        }
        _room_window->set_rooms(room_infos);
    }

    void Viewer::render()
    {
        _timer.update();

        _context->OMSetRenderTargets(1, &_render_target_view.p, _depth_stencil_view);
        _context->OMSetBlendState(_blend_state, 0, 0xffffffff);

        float colours[4] = { 0.f, 0.2f, 0.4f, 1.f };
        _context->ClearRenderTargetView(_render_target_view, colours);
        _context->ClearDepthStencilView(_depth_stencil_view, D3D11_CLEAR_DEPTH, 1.0f, 0);

        if (_level_textures.size())
        {
            render_scene();
        }
        render_ui();

        _swap_chain->Present(1, 0);
    }

    void Viewer::render_scene()
    {
        _context->OMSetDepthStencilState(_depth_stencil_state, 1);

        using namespace DirectX;

        // Update the view matrix based on the room selected in the room window.
        auto room = _current_level->get_room(_room_window->selected_room());

        XMVECTOR target_position = XMVectorSet(
            (room.info.x / 1024.f) + room.num_x_sectors / 2.f, 
            (room.info.yBottom / -1024.f) + (room.info.yTop - room.info.yBottom) / -1024.f / 2.0f,
            (room.info.z / 1024.f) + room.num_z_sectors / 2.f, 0);
        XMVECTOR eye_position = XMVectorSet(0, 8, -15, 0);

        static float x = 0;
        x += 1.0f * _timer.elapsed();

        auto rotate = XMMatrixRotationY(x);
        eye_position = XMVector3TransformCoord(eye_position, rotate) + target_position;

        XMVECTOR up_vector = XMVectorSet(0, 1, 0, 1);
        auto view = XMMatrixLookAtLH(eye_position, target_position, up_vector);

        float aspect_ratio = static_cast<float>(_width) / static_cast<float>(_height);
        auto projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect_ratio, 1.0f, 10000.0f);
        auto view_projection = view * projection;

        _context->PSSetSamplers(0, 1, &_sampler_state.p);
        _context->IASetInputLayout(_input_layout);
        _context->VSSetShader(_vertex_shader, nullptr, 0);
        _context->PSSetShader(_pixel_shader, nullptr, 0);
        _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        for(std::size_t i = 0; i < _level_rooms.size(); ++i)
        {
            _level_rooms[i]->render(_context, view_projection, _level_textures, _highlight || _room_window->selected_room() == i);
        }
    }

    void Viewer::render_ui()
    {
        // Render the user interface:
        _context->OMSetDepthStencilState(_ui_depth_stencil_state, 1);
        _texture_window->render(_context);
        _room_window->render(_context);
    }

    void Viewer::recalculate_size()
    {
        RECT rectangle;
        GetClientRect(_window, &rectangle);
        _width = rectangle.right - rectangle.left;
        _height = rectangle.bottom - rectangle.top;
    }

    void Viewer::cycle()
    {
        _texture_window->cycle();
    }

    void Viewer::cycle_room()
    {
        _room_window->cycle();
    }

    void Viewer::cycle_back()
    {
        _texture_window->cycle_back();
    }

    void Viewer::cycle_room_back()
    {
        _room_window->cycle_back();
    }

    void Viewer::toggle_room_window()
    {
        _room_window->toggle_visibility();
    }

    void Viewer::toggle_texture_window()
    {
        _texture_window->toggle_visibility();
    }

    void Viewer::toggle_highlight()
    {
        _highlight = !_highlight;
    }
}

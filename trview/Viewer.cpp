#include "stdafx.h"
#include "Viewer.h"
#include "..\trlevel\trlevel.h"

#include <vector>
#include <sstream>
#include <string>
#include <fstream>

#include <wincodec.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>

namespace trview
{
    namespace
    {
        struct Vertex
        {
            DirectX::XMFLOAT3 pos;
            DirectX::XMFLOAT2 uv;
        };
    }

    Viewer::Viewer(HWND window)
    {
        initialise_d3d(window);

        using namespace DirectX;

        Vertex vertices[] =
        {
            { XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0,0) },
            { XMFLOAT3(0.5f, 0.0f, 0.0f), XMFLOAT2(1,0) },
            { XMFLOAT3(0.0f, -0.5f, 0.0f), XMFLOAT2(0,1) },
            { XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT2(1,1) }
        };

        D3D11_BUFFER_DESC vertex_desc;
        memset(&vertex_desc, 0, sizeof(vertex_desc));
        vertex_desc.Usage = D3D11_USAGE_DEFAULT;
        vertex_desc.ByteWidth = sizeof(Vertex) * 4;
        vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertex_data;
        memset(&vertex_data, 0, sizeof(vertex_data));
        vertex_data.pSysMem = vertices;

        HRESULT hr = _device->CreateBuffer(&vertex_desc, &vertex_data, &_vertex_buffer);

        uint32_t indices[] = { 0, 1, 2, 3 };

        D3D11_BUFFER_DESC index_desc;
        memset(&index_desc, 0, sizeof(index_desc));
        index_desc.Usage = D3D11_USAGE_DEFAULT;
        index_desc.ByteWidth = sizeof(uint32_t) * 4;
        index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA index_data;
        memset(&index_data, 0, sizeof(index_data));
        index_data.pSysMem = indices;

        hr = _device->CreateBuffer(&index_desc, &index_data, &_index_buffer);

        std::ifstream shaderfile;
        shaderfile.open(L"VertexShader.cso", std::ios::binary);
        shaderfile.seekg(0, std::ios::end);
        std::size_t length = shaderfile.tellg();
        std::vector<char> data(length, 0);
        shaderfile.seekg(0, std::ios::beg);
        shaderfile.read(&data[0], length);
        shaderfile.close();

        D3D11_INPUT_ELEMENT_DESC input_desc[2];
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

        hr = _device->CreateInputLayout(input_desc, 2, &data[0], data.size(), &_input_layout);

        hr = _device->CreateVertexShader(&data[0], data.size(), nullptr, &_vertex_shader);

        std::ifstream pshaderfile;
        pshaderfile.open(L"PixelShader.cso", std::ios::binary);
        pshaderfile.seekg(0, std::ios::end);
        std::size_t plength = pshaderfile.tellg();
        std::vector<char> pdata(plength, 0);
        pshaderfile.seekg(0, std::ios::beg);
        pshaderfile.read(&pdata[0], plength);
        pshaderfile.close();

        hr = _device->CreatePixelShader(&pdata[0], pdata.size(), nullptr, &_pixel_shader);
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
                uint16_t r = t & 0x001f;
                uint16_t g = (t & 0x03e0) >> 5;
                uint16_t b = (t & 0x7c00) >> 10;

                r <<= 3;
                g <<= 3;
                b <<= 3;

                r += 3;
                g += 3;
                b += 3;

                data[index++] = 0xff << 24 | r << 16 | g << 8 | b;
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
            _level_textures.push_back(texture);
        }
    }

    void Viewer::render()
    {
        _context->OMSetRenderTargets(1, &_render_target_view.p, nullptr);

        float colours[4] = { 0.f, 0.2f, 0.4f, 1.f };
        _context->ClearRenderTargetView(_render_target_view, colours);

        if (!_level_textures.empty())
        {
            // Create a texture sampler state description.
            D3D11_SAMPLER_DESC desc;
            memset(&desc, 0, sizeof(desc));
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
            desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.MaxAnisotropy = 1;
            desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
            desc.MaxLOD = D3D11_FLOAT32_MAX;

            // Create the texture sampler state.
            CComPtr<ID3D11SamplerState> samplerState;
            _device->CreateSamplerState(&desc, &samplerState);

            CComPtr<ID3D11ShaderResourceView> view;
            _device->CreateShaderResourceView(_level_textures[0], nullptr, &view);
            _context->PSSetShaderResources(0, 1, &view.p);
            _context->PSSetSamplers(0, 1, &samplerState.p);

            // select which vertex buffer to display
            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            _context->IASetInputLayout(_input_layout);
            _context->IASetVertexBuffers(0, 1, &_vertex_buffer.p, &stride, &offset);
            _context->IASetIndexBuffer(_index_buffer, DXGI_FORMAT_R32_UINT, 0);
            _context->VSSetShader(_vertex_shader, nullptr, 0);
            _context->PSSetShader(_pixel_shader, nullptr, 0);
            _context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            _context->DrawIndexed(4, 0, 0);
        }

        _swap_chain->Present(1, 0);
    }
}

#include "stdafx.h"
#include "TextureWindow.h"
#include "FileLoader.h"

#include <DirectXMath.h>

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

    TextureWindow::TextureWindow(CComPtr<ID3D11Device> device, uint32_t host_width, uint32_t host_height)
        : _device(device), _host_width(host_width), _host_height(host_height)
    {
        create_scaling_matrix();

        using namespace DirectX;

        Vertex vertices[] =
        {
            { XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0,0) },
            { XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1,0) },
            { XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0,1) },
            { XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1,1) }
        };

        D3D11_BUFFER_DESC vertex_desc;
        memset(&vertex_desc, 0, sizeof(vertex_desc));
        vertex_desc.Usage = D3D11_USAGE_DEFAULT;
        vertex_desc.ByteWidth = sizeof(Vertex) * 4;
        vertex_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertex_data;
        memset(&vertex_data, 0, sizeof(vertex_data));
        vertex_data.pSysMem = vertices;

        HRESULT hr = device->CreateBuffer(&vertex_desc, &vertex_data, &_vertex_buffer);

        uint32_t indices[] = { 0, 1, 2, 3 };

        D3D11_BUFFER_DESC index_desc;
        memset(&index_desc, 0, sizeof(index_desc));
        index_desc.Usage = D3D11_USAGE_DEFAULT;
        index_desc.ByteWidth = sizeof(uint32_t) * 4;
        index_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

        D3D11_SUBRESOURCE_DATA index_data;
        memset(&index_data, 0, sizeof(index_data));
        index_data.pSysMem = indices;

        hr = device->CreateBuffer(&index_desc, &index_data, &_index_buffer);

        std::vector<char> vs_data = load_file(L"VertexShader.cso");

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

        hr = device->CreateInputLayout(input_desc, 2, &vs_data[0], vs_data.size(), &_input_layout);

        hr = device->CreateVertexShader(&vs_data[0], vs_data.size(), nullptr, &_vertex_shader);

        std::vector<char> ps_data = load_file(L"PixelShader.cso");
        hr = device->CreatePixelShader(&ps_data[0], ps_data.size(), nullptr, &_pixel_shader);

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
        device->CreateSamplerState(&desc, &_sampler_state);

        initialise_d2d();
    }

    void TextureWindow::initialise_d2d()
    {
        // Texture to render text to.
        D3D11_TEXTURE2D_DESC desc;
        desc.ArraySize = 1;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.Height = 512;
        desc.Width = 512;
        desc.MipLevels = 1;
        desc.MiscFlags = 0;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        _device->CreateTexture2D(&desc, nullptr, &_text_texture);

        D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &_d2d_factory);
        
        DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, 
            __uuidof(IDWriteFactory), 
            reinterpret_cast<IUnknown**>(&_dwrite_factory));

        _dwrite_factory->CreateTextFormat(
            L"Arial",
            nullptr,
            DWRITE_FONT_WEIGHT_REGULAR,
            DWRITE_FONT_STYLE_NORMAL,
            DWRITE_FONT_STRETCH_NORMAL,
            32.0f,
            L"en-us",
            &_text_format);

        _text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        _text_format->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

        CComPtr<IDXGISurface> surface;
        _text_texture->QueryInterface(&surface);

        D2D1_RENDER_TARGET_PROPERTIES props =
            D2D1::RenderTargetProperties(
                D2D1_RENDER_TARGET_TYPE_DEFAULT,
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
                96,
                96
            );

        HRESULT hr = _d2d_factory->CreateDxgiSurfaceRenderTarget(surface, &props, &_d2d_rt);

        _d2d_rt->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &_d2d_brush);
    }

    void TextureWindow::set_textures(std::vector<CComPtr<ID3D11ShaderResourceView>> textures)
    {
        _texture_index = 0u;
        _level_textures = textures;
    }

    void TextureWindow::render(CComPtr<ID3D11DeviceContext> context)
    {
        if (!_level_textures.empty())
        {
            context->PSSetShaderResources(0, 1, &_level_textures[_texture_index].p);
            context->PSSetSamplers(0, 1, &_sampler_state.p);

            // select which vertex buffer to display
            UINT stride = sizeof(Vertex);
            UINT offset = 0;
            context->IASetInputLayout(_input_layout);
            context->IASetVertexBuffers(0, 1, &_vertex_buffer.p, &stride, &offset);
            context->IASetIndexBuffer(_index_buffer, DXGI_FORMAT_R32_UINT, 0);
            context->VSSetShader(_vertex_shader, nullptr, 0);
            context->PSSetShader(_pixel_shader, nullptr, 0);
            context->VSSetConstantBuffers(0, 1, &_matrix_buffer.p);
            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
            context->DrawIndexed(4, 0, 0);

            // draw some text?
            std::wstring message = L"Please kill me";
            D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, 100, 100);

            _d2d_rt->BeginDraw();
            _d2d_rt->SetTransform(D2D1::IdentityMatrix());
            _d2d_rt->Clear(D2D1::ColorF(D2D1::ColorF(0, 0, 0, 0)));
            _d2d_rt->DrawText(
                message.c_str(),        // The string to render.
                message.size(),    // The string's length.
                _text_format,    // The text format.
                layoutRect,       // The region of the window where the text will be rendered.
                _d2d_brush     // The brush used to draw the text.
            );
            _d2d_rt->EndDraw();

            CComPtr<ID3D11ShaderResourceView> srv;
            _device->CreateShaderResourceView(_text_texture, nullptr, &srv);

            context->PSSetShaderResources(0, 1, &srv.p);
            context->DrawIndexed(4, 0, 0);
        }
    }

    void TextureWindow::cycle()
    {
        ++_texture_index;
        if (_texture_index >= _level_textures.size())
        {
            _texture_index = 0;
        }
    }

    void TextureWindow::set_host_size(uint32_t width, uint32_t height)
    {
        _host_width = width;
        _host_height = height;
        create_scaling_matrix();
    }

    void TextureWindow::create_scaling_matrix()
    {
        // Need to scale the quad so that it is a certain size. Will need to know the 
        // size of the host window as well as the size that we want the texture window
        // to be. Then create a scaling matrix and throw it in to the shader.
        const uint32_t target_width = 300;
        const uint32_t target_height = 300;

        auto scaling = DirectX::XMMatrixScaling(static_cast<float>(target_width) / _host_width,
            static_cast<float>(target_height) / _host_height, 1);

        // Try to make the appropriate translation matrix to move it to the top left of the screen.
        auto translation = DirectX::XMMatrixTranslation(
            -1.f + static_cast<float>(target_width) / _host_width,
            1.f - static_cast<float>(target_height) / _host_height, 
            0);

        scaling *= translation;

        D3D11_BUFFER_DESC desc;
        memset(&desc, 0, sizeof(desc));

        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.ByteWidth = sizeof(DirectX::XMMATRIX);
        desc.Usage = D3D11_USAGE_IMMUTABLE;

        D3D11_SUBRESOURCE_DATA data;
        memset(&data, 0, sizeof(data));

        data.pSysMem = &scaling;
        data.SysMemPitch = sizeof(scaling);

        _device->CreateBuffer(&desc, &data, &_matrix_buffer);
    }
}
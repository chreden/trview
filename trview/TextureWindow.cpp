#include "stdafx.h"
#include "TextureWindow.h"
#include "FileLoader.h"

#include <DirectXMath.h>

#include <sstream>

namespace trview
{
    TextureWindow::TextureWindow(CComPtr<ID3D11Device> device, uint32_t host_width, uint32_t host_height)
        : _device(device), _sprite(std::make_unique<Sprite>(device, host_width, host_height))
    {
        initialise_d2d();
        create_bg_texture();
    }

    void TextureWindow::create_bg_texture()
    {
        uint32_t pixel = 0xffffffff;
        D3D11_SUBRESOURCE_DATA srd;
        memset(&srd, 0, sizeof(srd));
        srd.pSysMem = &pixel;
        srd.SysMemPitch = sizeof(uint32_t);

        D3D11_TEXTURE2D_DESC desc;
        memset(&desc, 0, sizeof(desc));
        desc.Width = 1;
        desc.Height = 1;
        desc.MipLevels = desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;

        _device->CreateTexture2D(&desc, &srd, &_bg_texture);
        _device->CreateShaderResourceView(_bg_texture, nullptr, &_bg_resource);
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

        _text_format->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
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
        render_text();
    }

    void TextureWindow::render(CComPtr<ID3D11DeviceContext> context)
    {
        if (!_level_textures.empty())
        {
            _sprite->render(context, _bg_resource, _x, _y, 260, 300, DirectX::XMFLOAT4(0,0,0,1));
            _sprite->render(context, _level_textures[_texture_index], _x + 2, _y + 2, 256, 256);
            _sprite->render(context, _text_resource, _x + 2, _y + 258, 256, 256);
        }
    }

    void TextureWindow::cycle()
    {
        ++_texture_index;
        if (_texture_index >= _level_textures.size())
        {
            _texture_index = 0;
        }
        render_text();
    }

    void TextureWindow::render_text()
    {
        // draw some text?
        std::wstringstream stream;
        stream << L"Texture " << _texture_index + 1 << L"/" << _level_textures.size();
        std::wstring message = stream.str();
        D2D1_RECT_F layoutRect = D2D1::RectF(0, 0, 256, 100);

        _d2d_rt->BeginDraw();
        _d2d_rt->SetTransform(D2D1::IdentityMatrix());
        _d2d_rt->Clear(D2D1::ColorF(D2D1::ColorF(0, 0, 0, 0)));
        _d2d_rt->DrawText(message.c_str(), message.size(), _text_format, layoutRect, _d2d_brush);
        _d2d_rt->EndDraw();

        CComPtr<ID3D11ShaderResourceView> text_resource_view;
        _device->CreateShaderResourceView(_text_texture, nullptr, &text_resource_view);
        _text_resource = text_resource_view;
    }

    void TextureWindow::set_host_size(uint32_t width, uint32_t height)
    {
        _sprite->set_host_size(width, height);
    }
}
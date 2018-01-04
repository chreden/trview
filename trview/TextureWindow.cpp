#include "stdafx.h"
#include "TextureWindow.h"
#include "FileLoader.h"

#include <DirectXMath.h>

#include <sstream>

namespace trview
{
    TextureWindow::TextureWindow(CComPtr<ID3D11Device> device, FontFactory& font_factory, uint32_t host_width, uint32_t host_height)
        : _device(device), _sprite(std::make_unique<Sprite>(device, host_width, host_height)),
        _font(font_factory.create_font(device, L"Arial"))
    {
        create_bg_texture();
        _text_texture = _font->create_texture(_bg_texture);
    }

    void TextureWindow::create_bg_texture()
    {
        std::vector<uint32_t> pixels(260 * 300, 0xFF000000);

        D3D11_SUBRESOURCE_DATA srd;
        memset(&srd, 0, sizeof(srd));
        srd.pSysMem = &pixels[0];
        srd.SysMemPitch = sizeof(uint32_t) * 260;

        D3D11_TEXTURE2D_DESC desc;
        memset(&desc, 0, sizeof(desc));
        desc.Width = 260;
        desc.Height = 300;
        desc.MipLevels = desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        _device->CreateTexture2D(&desc, &srd, &_bg_texture.texture);
        _device->CreateShaderResourceView(_bg_texture.texture, nullptr, &_bg_texture.view);
        _device->CreateRenderTargetView(_bg_texture.texture, nullptr, &_render_target_view);
    }

    void TextureWindow::set_textures(std::vector<Texture> textures)
    {
        _texture_index = 0u;
        _level_textures = textures;
        render_text();
    }

    void TextureWindow::render(CComPtr<ID3D11DeviceContext> context)
    {
        if (!_visible)
        {
            return;
        }

        if (!_level_textures.empty())
        {
            // Only re-render the background texture if the selected texture has changed
            // and the text needs to be updated.
            if (_update_texture)
            {
                float colours[4] = { 0.0f, 0.0f, 0.0f, 1.f };
                context->ClearRenderTargetView(_render_target_view, colours);
                render_text();
                _update_texture = false;
            }

            _sprite->render(context, _bg_texture.view, _x, _y, 260, 300);
            _sprite->render(context, _level_textures[_texture_index].view, _x + 2, _y + 2, 256, 256);
        }
    }

    void TextureWindow::cycle()
    {
        if (_visible)
        {
            ++_texture_index;
            if (_texture_index >= _level_textures.size())
            {
                _texture_index = 0;
            }
            _update_texture = true;
        }
    }

    void TextureWindow::cycle_back()
    {
        if (_visible)
        {
            if (_texture_index == 0)
            {
                _texture_index = _level_textures.size() - 1;
            }
            else
            {
                --_texture_index;
            }
            _update_texture = true;
        }
    }

    void TextureWindow::render_text()
    {
        std::wstringstream stream;
        stream << L"Texture " << _texture_index + 1 << L"/" << _level_textures.size();
        std::wstring message = stream.str();
        _font->render(_text_texture, stream.str(), 0, 260);
    }

    void TextureWindow::set_host_size(uint32_t width, uint32_t height)
    {
        _sprite->set_host_size(width, height);
    }

    void TextureWindow::toggle_visibility()
    {
        _visible = !_visible;
    }

    bool TextureWindow::visible() const
    {
        return _visible;
    }
}
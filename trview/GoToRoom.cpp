#include "stdafx.h"
#include "GoToRoom.h"
#include "FontFactory.h"
#include <vector>
#include <sstream>

namespace trview
{
    namespace
    {
        const int Width = 50;
        const int Height = 30;
    }

    GoToRoom::GoToRoom(CComPtr<ID3D11Device> device, FontFactory& font_factory, uint32_t host_width, uint32_t host_height)
        : _device(device),
        _sprite(std::make_unique<Sprite>(device, host_width, host_height)),
        _font(font_factory.create_font(device, L"Arial", 15.f, DWRITE_TEXT_ALIGNMENT_CENTER, DWRITE_PARAGRAPH_ALIGNMENT_CENTER)),
        _host_width(host_width), _host_height(host_height)
    {
        create_bg_texture();
        _text_texture = _font->create_texture(_bg_texture);
    }

    void GoToRoom::render(CComPtr<ID3D11DeviceContext> context)
    {
        // Check here for whether the goto room box is visible or not.
        if (!visible())
        {
            return;
        }

        if (_update_texture)
        {
            float colours[4] = { 0.25f, 0.25f, 0.25f, 1.f };
            context->ClearRenderTargetView(_render_target_view, colours);
            render_text();
            _update_texture = false;
        }

        // Centre the go to room box.
        const float x = _host_width / 2.f - Width / 2.f;
        const float y = _host_height / 2.f - Height / 2.f;
        _sprite->render(context, _bg_texture.view, x, y, Width, Height);
    }

    bool GoToRoom::visible() const
    {
        return _visible;
    }

    void GoToRoom::toggle_visible()
    {
        _visible = !_visible;
        if (_visible)
        {
            _input.clear();
            _update_texture = true;
        }
    }

    void GoToRoom::character(uint16_t character)
    {
        if (!visible())
        {
            return;
        }

        auto value = character - '0';
        if (value >= 0 && value <= 9)
        {
            _input += character;
            _update_texture = true;
        }
    }

    void GoToRoom::input(uint16_t key)
    {
        if (!visible())
        {
            return;
        }

        if (key == VK_RETURN)
        {
            std::wstringstream stream(_input);
            uint32_t room = 0;
            stream >> room;
            room_selected.raise(room);
            toggle_visible();
        }
        else if (key == VK_ESCAPE)
        {
            toggle_visible();
        }
    }

    void GoToRoom::create_bg_texture()
    {
        std::vector<uint32_t> pixels(Width * Height, 0xFF808080);

        D3D11_SUBRESOURCE_DATA srd;
        memset(&srd, 0, sizeof(srd));
        srd.pSysMem = &pixels[0];
        srd.SysMemPitch = sizeof(uint32_t) * Width;

        D3D11_TEXTURE2D_DESC desc;
        memset(&desc, 0, sizeof(desc));
        desc.Width = Width;
        desc.Height = Height;
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

    void GoToRoom::render_text()
    {
        _font->render(_text_texture, _input, 0, 0, Width, Height);
    }
}

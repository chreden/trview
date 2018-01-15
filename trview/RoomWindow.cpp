#include "stdafx.h"
#include "RoomWindow.h"
#include <vector>
#include <sstream>

namespace trview
{
    RoomWindow::RoomWindow(CComPtr<ID3D11Device> device, FontFactory& font_factory, uint32_t host_width, uint32_t host_height)
        : _device(device), _sprite(std::make_unique<Sprite>(device, host_width, host_height)), _font(font_factory.create_font(device, L"Arial"))
    {
        create_bg_texture();

        // Prepare the BG texture for being drawn all over.
        _font_texture = _font->create_texture(_bg_texture);
    }

    void RoomWindow::create_bg_texture()
    {
        std::vector<uint32_t> pixels(256 * 256, 0xFF000000);

        D3D11_SUBRESOURCE_DATA srd;
        memset(&srd, 0, sizeof(srd));
        srd.pSysMem = &pixels[0];
        srd.SysMemPitch = sizeof(uint32_t) * 256;

        D3D11_TEXTURE2D_DESC desc;
        memset(&desc, 0, sizeof(desc));
        desc.Width = 256;
        desc.Height = 256;
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

    void RoomWindow::render(CComPtr<ID3D11DeviceContext> context)
    {
        if (_visible)
        {
            // Only re-render the background texture if the selected room has changed.
            if (_update_texture)
            {
                float colours[4] = { 0.0f, 0.0f, 0.0f, 1.f };
                context->ClearRenderTargetView(_render_target_view, colours);
                render_text();
                _update_texture = false;
            }

            _sprite->render(context, _bg_texture.view, 400, 10, 256, 256);
        }
    }

    void RoomWindow::set_rooms(std::vector<RoomInfo> rooms)
    {
        _rooms = rooms;
        _room_index = 0u;
        _update_texture = true;
    }

    void RoomWindow::render_text()
    {
        _font->render(_font_texture, L"Room Window", 0, 0);

        if (!_rooms.empty())
        {
            auto info = _rooms[_room_index];
            
            std::wstringstream stream;
            stream << L"Room " << _room_index << L"/" << _rooms.size() - 1 << L": " << info.x / 1024 << L',' << info.z / 1024 << L'\n';
            _font->render(_font_texture, stream.str(), 0, 30);
        }
    }

    void RoomWindow::cycle()
    {
        ++_room_index;
        if (_room_index >= _rooms.size())
        {
            _room_index = 0u;
        }
        _update_texture = true;
    }

    void RoomWindow::cycle_back()
    {
        if (_room_index == 0)
        {
            _room_index = _rooms.size() - 1;
        }
        else
        {
            --_room_index;
        }
        _update_texture = true;
    }

    void RoomWindow::toggle_visibility()
    {
        _visible = !_visible;
    }

    bool RoomWindow::visible() const
    {
        return _visible;
    }

    uint32_t RoomWindow::selected_room() const
    {
        return _room_index;
    }
    
    void RoomWindow::select_room(uint32_t room)
    {
        if (_room_index < _rooms.size())
        {
            _room_index = room;
            _update_texture = true;
        }
    }
}

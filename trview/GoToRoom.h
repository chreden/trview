#pragma once

#include <atlbase.h>
#include <d3d11.h>
#include <memory>

#include "Sprite.h"
#include "Font.h"
#include "Texture.h"

#include <trview.common/Event.h>

namespace trview
{
    class FontFactory;
    
    // This window presents the user with a box where they can enter the room number
    // that they want to go to. Then when they press enter, that will be the selected
    // room.
    class GoToRoom
    {
    public:
             GoToRoom(CComPtr<ID3D11Device> device, FontFactory& font_factory, uint32_t host_width, uint32_t host_height);
        void render(CComPtr<ID3D11DeviceContext> context);
        bool visible() const;
        void toggle_visible();

        void character(uint16_t character);
        void input(uint16_t key);

        Event<uint32_t> room_selected;
    private:
        void create_bg_texture();
        void render_text();

        CComPtr<ID3D11Device>           _device;
        std::unique_ptr<Sprite>         _sprite;
        std::unique_ptr<Font>           _font;
        Texture                         _bg_texture;
        FontTexture                     _text_texture;
        CComPtr<ID3D11RenderTargetView> _render_target_view;
        std::wstring                    _input;
        bool                            _visible{ false };
        bool                            _update_texture{ true };
        uint32_t                        _host_width;
        uint32_t                        _host_height;
    };
}

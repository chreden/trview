#pragma once

#include <memory>
#include <cstdint>
#include <atlbase.h>
#include <d3d11.h>

#include "Sprite.h"
#include "Texture.h"
#include "Font.h"
#include "FontFactory.h"
#include "Room.h"

#include <trlevel/trtypes.h>

namespace trview
{
    class RoomWindow
    {
    public:
        RoomWindow(CComPtr<ID3D11Device> device, FontFactory& font_factory, uint32_t host_width, uint32_t host_height);

        void render(CComPtr<ID3D11DeviceContext> context);

        void set_rooms(std::vector<RoomInfo> rooms);

        void toggle_visibility();

        void cycle();

        void cycle_back();

        bool visible() const;

        uint32_t selected_room() const;
    private:
        void create_bg_texture();

        void render_text();

        CComPtr<ID3D11Device>   _device;
        std::unique_ptr<Sprite> _sprite;
        Texture                 _bg_texture;
        FontTexture             _font_texture;
        std::unique_ptr<Font>   _font;
        bool                    _update_texture{ true };
        CComPtr<ID3D11RenderTargetView> _render_target_view;

        bool                    _visible{ true };

        std::vector<RoomInfo>   _rooms;
        uint32_t                _room_index{ 0u };
    };
}

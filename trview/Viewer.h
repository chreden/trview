#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <atlbase.h>

#include <vector>
#include <string>
#include <memory>

#include "..\trlevel\ILevel.h"

#include "Texture.h"
#include "Room.h"

#include "TextureWindow.h"
#include "RoomWindow.h"

#include "FontFactory.h"

namespace trview
{
    class Viewer
    {
    public:
        explicit Viewer(HWND window);

        void render();

        void open(const std::wstring filename);

        void toggle_room_window();
        void toggle_texture_window();

        void cycle();
        void cycle_room();
    private:
        void initialise_d3d(HWND window);

        // Generate the textures that will be used to render the level based on the
        // textiles loaded in the level.
        void generate_textures();

        void generate_rooms();

        CComPtr<IDXGISwapChain>         _swap_chain;
        CComPtr<ID3D11Device>           _device;
        CComPtr<ID3D11DeviceContext>    _context;
        CComPtr<ID3D11RenderTargetView> _render_target_view;
        
        std::unique_ptr<trlevel::ILevel> _current_level;
        std::unique_ptr<TextureWindow>   _texture_window;

        std::unique_ptr<FontFactory> _font_factory;
        std::unique_ptr<RoomWindow> _room_window;
        CComPtr<ID3D11BlendState> _blend_state;

        // The 'view' bits, so to speak.
        std::vector<Texture> _level_textures;
        std::vector<Room> _level_rooms;
    };
}

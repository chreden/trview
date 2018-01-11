#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <atlbase.h>
#include <cstdint>

#include <vector>
#include <string>
#include <memory>

#include "..\trlevel\ILevel.h"

#include "Texture.h"
#include "Room.h"

#include "TextureWindow.h"
#include "RoomWindow.h"

#include "FontFactory.h"
#include "Timer.h"

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
        void cycle_back();
        void cycle_room_back();
        void toggle_highlight();
    private:
        void initialise_d3d(HWND window);

        // Generate the textures that will be used to render the level based on the
        // textiles loaded in the level.
        void generate_textures();
        void generate_rooms();
        // Draw the 3d elements of the scene.
        void render_scene();
        // Draw the user interface elements of the scene.
        void render_ui();
        // Update the size of the window so the renderer can have the correct perspective.
        void recalculate_size();

        CComPtr<IDXGISwapChain>          _swap_chain;
        CComPtr<ID3D11Device>            _device;
        CComPtr<ID3D11DeviceContext>     _context;
        CComPtr<ID3D11RenderTargetView>  _render_target_view;
        CComPtr<ID3D11Texture2D>         _depth_stencil_buffer;
        CComPtr<ID3D11DepthStencilState> _depth_stencil_state;
        CComPtr<ID3D11DepthStencilState> _ui_depth_stencil_state;
        CComPtr<ID3D11DepthStencilView>  _depth_stencil_view;
        
        std::unique_ptr<trlevel::ILevel> _current_level;
        std::unique_ptr<TextureWindow>   _texture_window;

        std::unique_ptr<FontFactory> _font_factory;
        std::unique_ptr<RoomWindow> _room_window;
        CComPtr<ID3D11BlendState> _blend_state;

        // The 'view' bits, so to speak.
        std::vector<Texture> _level_textures;
        std::vector<std::unique_ptr<Room>> _level_rooms;

        HWND     _window;
        uint32_t _width;
        uint32_t _height;

        CComPtr<ID3D11VertexShader> _vertex_shader;
        CComPtr<ID3D11PixelShader>  _pixel_shader;
        CComPtr<ID3D11InputLayout>  _input_layout;
        CComPtr<ID3D11SamplerState> _sampler_state;

        Timer _timer;
        bool _highlight{ false };
    };
}


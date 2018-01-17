#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <atlbase.h>
#include <cstdint>

#include <vector>
#include <string>
#include <memory>

#include <trlevel/ILevel.h>

#include <trview.common/Texture.h>
#include "Room.h"

#include "TextureWindow.h"
#include "RoomWindow.h"
#include "GoToRoom.h"

#include "Timer.h"
#include "Camera.h"
#include "Window.h"

#include <trview.input/Keyboard.h>
#include <trview.input/Mouse.h>
#include <trview.ui/Control.h>

#include <trview.ui.render/Renderer.h>
#include <trview.ui.render/FontFactory.h>

namespace trview
{
    namespace ui
    {
        class Label;
        class Button;
    }

    class Viewer
    {
    public:
        explicit Viewer(Window window);

        void render();

        void open(const std::wstring filename);

        void on_char(uint16_t character);
        void on_key_down(uint16_t key);
        void on_key_up(uint16_t key);
        void on_input(const RAWINPUT& input);

        // Old ways of doing things - will be mapped.
        void toggle_room_window();
        void toggle_texture_window();

        void cycle();
        void cycle_room();
        void cycle_back();
        void cycle_room_back();
        void toggle_highlight();
    private:
        void generate_ui();

        void initialise_d3d();
        void initialise_input();
        void process_input_key(uint16_t key);
        void process_char(uint16_t character);

        void update_camera();

        // Generate the textures that will be used to render the level based on the
        // textiles loaded in the level.
        void generate_textures();
        void generate_rooms();
        // Draw the 3d elements of the scene.
        void render_scene();
        // Draw the user interface elements of the scene.
        void render_ui();

        Texture create_coloured_texture(uint32_t colour);

        CComPtr<IDXGISwapChain>          _swap_chain;
        CComPtr<ID3D11Device>            _device;
        CComPtr<ID3D11DeviceContext>     _context;
        CComPtr<ID3D11RenderTargetView>  _render_target_view;
        CComPtr<ID3D11Texture2D>         _depth_stencil_buffer;
        CComPtr<ID3D11DepthStencilState> _depth_stencil_state;
        CComPtr<ID3D11DepthStencilView>  _depth_stencil_view;
        
        std::unique_ptr<trlevel::ILevel> _current_level;
        std::unique_ptr<TextureWindow>   _texture_window;

        std::unique_ptr<ui::render::FontFactory> _font_factory;
        std::unique_ptr<RoomWindow> _room_window;
        CComPtr<ID3D11BlendState> _blend_state;

        // The 'view' bits, so to speak.
        std::vector<Texture> _level_textures;
        std::vector<std::unique_ptr<Room>> _level_rooms;

        Window _window;

        CComPtr<ID3D11VertexShader> _vertex_shader;
        CComPtr<ID3D11PixelShader>  _pixel_shader;
        CComPtr<ID3D11InputLayout>  _input_layout;
        CComPtr<ID3D11SamplerState> _sampler_state;

        Timer _timer;
        bool _highlight{ false };

        
        Camera _camera;
        input::Keyboard _keyboard;
        input::Mouse _mouse;

        // Camera rotation variables - eventually to be moved to a camera class.
        bool _rotating{ false };

        // Room navigator code.
        std::unique_ptr<GoToRoom> _go_to_room;

        // New user interface control structure.
        std::unique_ptr<ui::Control> _control;
        std::unique_ptr<ui::render::Renderer> _ui_renderer;

        ui::Button* _room_highlight;

        // Test of buttons
        bool _room_neighbours{ false };

    };
}


#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <atlbase.h>
#include <cstdint>

#include <string>
#include <memory>

#include <trlevel/ILevel.h>

#include <trview.common/Texture.h>
#include "Level.h"
#include "Room.h"

#include "TextureWindow.h"
#include "GoToRoom.h"

#include "Timer.h"
#include "Camera.h"
#include "FreeCamera.h"
#include "Window.h"
#include "UserSettings.h"
#include "CameraMode.h"

#include <trview.input/Keyboard.h>
#include <trview.input/Mouse.h>

#include <trview.ui.render/Renderer.h>
#include <trview.ui.render/FontFactory.h>

#include "trview.ui.render/MapRenderer.h"

namespace trview
{
    namespace ui
    {
        class Control;
        class Label;
    }

    class RoomNavigator;
    class CameraControls;
    class Neighbours;
    class LevelInfo;
    struct ITextureStorage;

    class Viewer
    {
    public:
        explicit Viewer(Window window);
        ~Viewer();

        void render();

        void open(const std::wstring filename);

        void on_char(uint16_t character);
        void on_key_down(uint16_t key);
        void on_key_up(uint16_t key);
        void on_input(const RAWINPUT& input);

        // Old ways of doing things - will be mapped.
        void toggle_texture_window();

        void cycle();
        void cycle_back();
        void toggle_highlight();

        UserSettings settings() const;
        Event<std::list<std::wstring>> on_recent_files_changed;
    private:
        void generate_ui();
        void generate_tool_window();

        void initialise_d3d();
        void initialise_input();
        void process_input_key(uint16_t key);
        void process_char(uint16_t character);

        void update_camera();

        // Draw the 3d elements of the scene.
        void render_scene();
        // Draw the user interface elements of the scene.
        void render_ui();
        // Draws a minimap 
        void render_map(); 

        void select_room(uint32_t room);

        // Determines whether the cursor is over a UI element that would take any input.
        // Returns: True if there is any UI under the cursor that would take input.
        bool over_ui() const;

        inline bool over_map() const { return _map_renderer->loaded() && _map_renderer->cursor_is_over_control(); }

        void pick();

        const ICamera& current_camera() const;
        ICamera& current_camera();
        void set_camera_mode(CameraMode camera_mode);

        void set_alternate_mode(bool enabled);

        CComPtr<IDXGISwapChain>          _swap_chain;
        CComPtr<ID3D11Device>            _device;
        CComPtr<ID3D11DeviceContext>     _context;
        CComPtr<ID3D11RenderTargetView>  _render_target_view;
        CComPtr<ID3D11Texture2D>         _depth_stencil_buffer;
        CComPtr<ID3D11DepthStencilState> _depth_stencil_state;
        CComPtr<ID3D11DepthStencilView>  _depth_stencil_view;

        std::unique_ptr<TextureWindow>   _texture_window;

        std::unique_ptr<ui::render::FontFactory> _font_factory;
        CComPtr<ID3D11BlendState> _blend_state;

        std::unique_ptr<trlevel::ILevel> _current_level;
        std::unique_ptr<Level> _level;

        Window _window;

        Timer _timer;

        Camera _camera;
        FreeCamera _free_camera;

        input::Keyboard _keyboard;
        input::Mouse _mouse;
        float _camera_sensitivity{ 0.0f };
        float _camera_movement_speed{ 0.0f };

        const float _CAMERA_MOVEMENT_SPEED_MULTIPLIER = 23.0f;
        const float _CAMERA_MOVEMENT_SPEED_DEFAULT = 0.5f;

        // Camera rotation variables - eventually to be moved to a camera class.
        bool _rotating{ false };

        // Room navigator code.
        std::unique_ptr<GoToRoom> _go_to_room;

        // New user interface control structure.
        std::unique_ptr<ui::Control> _control;
        std::unique_ptr<ui::render::Renderer> _ui_renderer;

        std::unique_ptr<ui::render::MapRenderer> _map_renderer;

        CameraMode _camera_mode{ CameraMode::Orbit };
        bool _free_forward{ false };
        bool _free_left{ false };
        bool _free_right{ false };
        bool _free_backward{ false };
        bool _free_up{ false };
        bool _free_down{ false };

        std::unique_ptr<RoomNavigator> _room_navigator;
        std::unique_ptr<CameraControls> _camera_controls;
        std::unique_ptr<Neighbours> _neighbours;
        std::unique_ptr<LevelInfo> _level_info;
        std::unique_ptr<ITextureStorage> _texture_storage;

        UserSettings _settings;

        // Picking
        ui::Label*        _picking;
        Level::PickResult _current_pick;
    };
}


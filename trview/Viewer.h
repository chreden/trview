#pragma once

#include <Windows.h>
#include <cstdint>

#include <string>
#include <memory>

#include <trlevel/ILevel.h>

#include "Level.h"
#include "Room.h"

#include "TextureWindow.h"
#include "GoToRoom.h"

#include <trview.common/Timer.h>
#include "Camera.h"
#include "FreeCamera.h"
#include <trview.common/Window.h>
#include "UserSettings.h"
#include "CameraMode.h"

#include <trview.input/Keyboard.h>
#include <trview.input/Mouse.h>
#include <trview.graphics/Device.h>

#include "CameraInput.h"

namespace trview
{
    namespace ui
    {
        class Control;
        class Label;

        namespace render
        {
            class Renderer;
            class MapRenderer;
        }
    }

    class RoomNavigator;
    class CameraControls;
    class Neighbours;
    class LevelInfo;
    struct ITextureStorage;

    namespace graphics
    {
        struct IShaderStorage;
    }

    class Viewer
    {
    public:
        explicit Viewer(Window window);
        ~Viewer();

        void render();

        void open(const std::wstring filename);

        UserSettings settings() const;

        Event<std::wstring> on_file_loaded;

        Event<std::list<std::wstring>> on_recent_files_changed;

        // Resize the window and the rendering system.
        void resize();
    private:
        void generate_ui();
        void generate_tool_window();
        void initialise_input();
        void process_input_key(uint16_t key);
        void process_char(uint16_t character);
        void toggle_highlight();

        void update_camera();

        // Draw the 3d elements of the scene.
        void render_scene();
        // Draws a minimap 
        void render_map(); 

        void select_room(uint32_t room);

        // Determines whether the cursor is over a UI element that would take any input.
        // Returns: True if there is any UI under the cursor that would take input.
        bool over_ui() const;
        bool over_map() const;

        void pick();

        const ICamera& current_camera() const;
        ICamera& current_camera();
        void set_camera_mode(CameraMode camera_mode);

        void set_alternate_mode(bool enabled);

        // Tell things that need to be resized that they should resize.
        void resize_elements();

        // Set up keyboard and mouse input for the camera.
        void setup_camera_input();

        graphics::Device _device;

        std::unique_ptr<TextureWindow>   _texture_window;

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

        // Room navigator code.
        std::unique_ptr<GoToRoom> _go_to_room;

        // New user interface control structure.
        std::unique_ptr<ui::Control> _control;
        std::unique_ptr<ui::render::Renderer> _ui_renderer;
        std::unique_ptr<ui::render::MapRenderer> _map_renderer;

        CameraMode _camera_mode{ CameraMode::Orbit };
        CameraInput _camera_input;

        std::unique_ptr<RoomNavigator> _room_navigator;
        std::unique_ptr<CameraControls> _camera_controls;
        std::unique_ptr<Neighbours> _neighbours;
        std::unique_ptr<LevelInfo> _level_info;
        std::unique_ptr<ITextureStorage> _texture_storage;
        std::unique_ptr<graphics::IShaderStorage> _shader_storage;

        UserSettings _settings;

        // Picking
        ui::Label*        _picking;
        Level::PickResult _current_pick;
    };
}


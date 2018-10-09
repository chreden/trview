/// @file Viewer.h
/// @brief Class that brings together the component parts of the application.
/// 
/// Class that coordinates all the parts of the application.

#pragma once

#include <Windows.h>
#include <cstdint>
#include <memory>
#include <string>

#include <trlevel/ILevel.h>
#include <trview.common/Timer.h>
#include <trview.common/Window.h>
#include <trview.graphics/Device.h>
#include <trview.input/Keyboard.h>
#include <trview.input/Mouse.h>
#include <trview.common/TokenStore.h>

#include "Camera.h"
#include "CameraInput.h"
#include "CameraMode.h"
#include "FreeCamera.h"
#include "Level.h"
#include "UserSettings.h"
#include "LevelSwitcher.h"
#include <trview.app/WindowResizer.h>
#include <trview.app/RecentFiles.h>
#include <trview.app/FileDropper.h>
#include <trview.app/ItemsWindowManager.h>
#include <trview.app/TriggersWindowManager.h>

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

    class CameraControls;
    class GoToRoom;
    struct ITextureStorage;
    class LevelInfo;
    class Neighbours;
    class RoomNavigator;
    class SettingsWindow;

    namespace graphics
    {
        struct IShaderStorage;
        class FontFactory;
        class DeviceWindow;
    }

    /// Class that coordinates all the parts of the application.
    class Viewer
    {
    public:
        /// Create a new viewer.
        /// @param window The window that the viewer should use.
        explicit Viewer(const Window& window);

        /// Destructor for the viewer.
        ~Viewer();

        /// Render the viewer.
        void render();

        /// Attempt to open the specified level file.
        /// @param filename The level file to open.
        void open(const std::wstring filename);

        /// Get the current user settings.
        /// @returns The current settings.
        UserSettings settings() const;

        /// Event raised when a level file is successfully opened.
        /// @remarks The filename is passed as a parameter to the listener functions.
        Event<std::wstring> on_file_loaded;

        /// Event raised when the recent files list is updated.
        /// @remarks The list of filenames is passed as a parameter to the listener functions.
        Event<std::list<std::wstring>> on_recent_files_changed;
    private:
        void generate_ui();
        void generate_tool_window();
        void initialise_input();
        void process_input_key(uint16_t key);
        void process_char(uint16_t character);
        void toggle_highlight();
        void update_camera();
        void render_scene();
        void render_map(); 
        void select_room(uint32_t room);
        void select_item(const Item& item);
        void select_trigger(const Trigger& trigger);
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
        std::unique_ptr<graphics::DeviceWindow> _main_window;
        std::unique_ptr<ItemsWindowManager> _items_windows;
        std::unique_ptr<TriggersWindowManager> _triggers_windows;
        std::unique_ptr<trlevel::ILevel> _current_level;
        std::unique_ptr<Level> _level;
        Window _window;
        Timer _timer;
        Camera _camera;
        FreeCamera _free_camera;
        input::Keyboard _keyboard;
        input::Mouse _mouse;
        std::unique_ptr<GoToRoom> _go_to_room;
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
        std::unique_ptr<graphics::FontFactory> _font_factory;
        std::unique_ptr<SettingsWindow> _settings_window;
        UserSettings _settings;
        ui::Label* _picking;
        PickResult _current_pick;
        LevelSwitcher _level_switcher;
        WindowResizer _window_resizer;
        RecentFiles _recent_files;
        FileDropper _file_dropper;
        TokenStore _token_store;
        DirectX::SimpleMath::Vector3 _target;
    };
}


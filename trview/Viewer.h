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

#include <trview.app/FreeCamera.h>
#include <trview.app/OrbitCamera.h>
#include "CameraInput.h"
#include "CameraMode.h"
#include "Level.h"
#include "UserSettings.h"
#include <trview.app/LevelSwitcher.h>
#include <trview.app/WindowResizer.h>
#include <trview.app/RecentFiles.h>
#include <trview.app/FileDropper.h>
#include <trview.app/ItemsWindowManager.h>
#include <trview.app/TriggersWindowManager.h>
#include <trview.app/Toolbar.h>
#include <trview.app/Measure.h>
#include <trview.app/Compass.h>
#include <trview.app/AlternateGroupToggler.h>
#include <trview.app/ContextMenu.h>
#include <trview.app/Route.h>
#include <trview.app/RouteWindowManager.h>
#include <trview.app/ViewMenu.h>
#include <trview.app/Picking.h>
#include <trview.app/SectorHighlight.h>

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
    class Flipmaps;

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
        void open(const std::string& filename);

        /// Get the current user settings.
        /// @returns The current settings.
        UserSettings settings() const;

        /// Event raised when a level file is successfully opened.
        /// @remarks The filename is passed as a parameter to the listener functions.
        Event<std::string> on_file_loaded;

        /// Event raised when the recent files list is updated.
        /// @remarks The list of filenames is passed as a parameter to the listener functions.
        Event<std::list<std::string>> on_recent_files_changed;
    private:
        void generate_ui();
        void generate_tool_window();
        void initialise_camera_controls(ui::Control& parent);
        void initialise_input();
        void process_input_key(uint16_t key);
        void process_char(uint16_t character);
        void toggle_highlight();
        void update_camera();
        void render_scene();
        void render_map(); 
        void select_room(uint32_t room);
        void select_item(const Item& item);
        void select_trigger(const Trigger* const trigger);
        void select_waypoint(uint32_t index);
        void remove_waypoint(uint32_t index);
        // Determines whether the cursor is over a UI element that would take any input.
        // Returns: True if there is any UI under the cursor that would take input.
        bool over_ui() const;
        bool over_map() const;
        bool should_pick() const;
        const ICamera& current_camera() const;
        ICamera& current_camera();
        void set_camera_mode(CameraMode camera_mode);
        void set_alternate_mode(bool enabled);
        void set_alternate_group(uint16_t group, bool enabled);
        bool alternate_group(uint16_t group) const;
        // Tell things that need to be resized that they should resize.
        void resize_elements();
        // Set up keyboard and mouse input for the camera.
        void setup_camera_input();
        void set_show_triggers(bool show);
        void set_show_hidden_geometry(bool show);
        void set_show_water(bool show);
        uint32_t room_from_pick(const PickResult& pick) const;

        graphics::Device _device;
        std::unique_ptr<graphics::DeviceWindow> _main_window;
        std::unique_ptr<ItemsWindowManager> _items_windows;
        std::unique_ptr<TriggersWindowManager> _triggers_windows;
        std::unique_ptr<trlevel::ILevel> _current_level;
        std::unique_ptr<Level> _level;
        Window _window;
        Timer _timer;
        OrbitCamera _camera;
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
        std::unique_ptr<Flipmaps> _flipmaps;
        std::unique_ptr<CameraControls> _camera_controls;
        std::unique_ptr<Neighbours> _neighbours;
        std::unique_ptr<LevelInfo> _level_info;
        std::unique_ptr<ITextureStorage> _texture_storage;
        std::unique_ptr<graphics::IShaderStorage> _shader_storage;
        graphics::FontFactory _font_factory;
        std::unique_ptr<SettingsWindow> _settings_window;
        UserSettings _settings;
        std::unique_ptr<Picking> _picking;
        PickResult _current_pick;
        LevelSwitcher _level_switcher;
        WindowResizer _window_resizer;
        RecentFiles _recent_files;
        FileDropper _file_dropper;
        TokenStore _token_store;
        AlternateGroupToggler _alternate_group_toggler;
        DirectX::SimpleMath::Vector3 _target;
        ViewMenu _view_menu;
        bool _show_selection{ true };
        SectorHighlight _sector_highlight;

        // Tools:
        std::unique_ptr<Toolbar> _toolbar;
        std::unique_ptr<ContextMenu> _context_menu;

        enum class Tool
        {
            None,
            Measure
        };

        Tool _active_tool{ Tool::None };
        std::unique_ptr<Measure> _measure;
        std::unique_ptr<Compass> _compass;
        std::optional<Compass::Axis> _compass_axis;

        // Temporary route objects.
        PickResult _context_pick;
        std::unique_ptr<Route> _route;
        std::unique_ptr<RouteWindowManager> _route_window_manager;
        bool _show_route{ true };
    };
}


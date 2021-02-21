/// @file Viewer.h
/// @brief Class that brings together the component parts of the application.
/// 
/// Class that coordinates all the parts of the application.

#pragma once

#include <Windows.h>
#include <cstdint>
#include <memory>
#include <string>

#include <trview.common/Timer.h>
#include <trview.common/Window.h>
#include <trview.graphics/Device.h>
#include <trview.input/Keyboard.h>
#include <trview.input/Mouse.h>
#include <trview.common/TokenStore.h>

#include <trview.app/Camera/FreeCamera.h>
#include <trview.app/Camera/OrbitCamera.h>
#include <trview.app/Camera/CameraInput.h>
#include <trview.app/Camera/CameraMode.h>
#include <trview.app/Elements/Level.h>
#include <trview.app/Settings/UserSettings.h>
#include <trview.app/Windows/WindowResizer.h>
#include <trview.app/Tools/Measure.h>
#include <trview.app/Tools/Compass.h>
#include <trview.app/Menus/AlternateGroupToggler.h>
#include <trview.app/Routing/Route.h>
#include <trview.app/Geometry/IPicking.h>
#include <trview.app/Graphics/SectorHighlight.h>
#include <trview.app/UI/IViewerUI.h>
#include <trview.app/Elements/ITypeNameLookup.h>
#include <trview.app/Menus/MenuDetector.h>
#include <trview.app/Lua/Lua.h>
#include <trview.common/Windows/Shortcuts.h>
#include <trview.graphics/DeviceWindow.h>

namespace trview
{
    struct ITextureStorage;

    namespace graphics
    {
        struct IShaderStorage;
        struct IFontFactory;
        class DeviceWindow;
    }

    /// Class that coordinates all the parts of the application.
    class Viewer
    {
    public:
        /// Create a new viewer.
        /// @param window The window that the viewer should use.
        explicit Viewer(
            const Window& window, 
            graphics::Device& device, 
            const graphics::IShaderStorage& shader_storage, 
            std::unique_ptr<IViewerUI> ui, 
            std::unique_ptr<IPicking> picking,
            std::unique_ptr<input::IMouse> mouse,
            Shortcuts& shortcuts, 
            Route* route);

        /// Destructor for the viewer.
        virtual ~Viewer() = default;

        /// Render the viewer.
        void render();

        /// Attempt to open the specified level file.
        /// @param filename The level file to open.
        void open(ILevel* level);

        /// Event raised when the user settings have changed.
        /// @remarks The settings is passed as a parameter to the listener functions.
        Event<UserSettings> on_settings;

        /// Event raised when the viwer wants to select an item.
        Event<Item> on_item_selected;

        /// Event raised when the viewer wants to change the visibility of an item.
        Event<Item, bool> on_item_visibility;

        /// Event raised when the viewer wants to select a room.
        Event<uint32_t> on_room_selected;

        /// Event raised when the viewer wants to select a trigger.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when the viewer wants to change the visibility of a trigger.
        Event<Trigger*, bool> on_trigger_visibility;

        /// Event raised when the viewer wants to select a waypoint.
        Event<uint32_t> on_waypoint_selected;

        /// Event raised when the viewer wants to remove a waypoint.
        Event<uint32_t> on_waypoint_removed;

        /// Event raised when the viewer wants to add a waypoint.
        Event<DirectX::SimpleMath::Vector3, uint32_t, Waypoint::Type, uint32_t> on_waypoint_added;

        void set_settings(const UserSettings& settings);

        /// Select the specified item.
        /// @param item The item to select.
        /// @remarks This will not raise the on_item_selected event.
        void select_item(const Item& item);

        /// Select the specified room.
        /// @param room The room to select.
        /// @remarks This will not raise the on_room_selected event.
        void select_room(uint32_t room);

        /// Select the specified trigger.
        /// @param trigger The trigger to select.
        /// @remarks This will not raise the on_trigger_selected event.
        void select_trigger(const Trigger* const trigger);

        /// Select the specified waypoint
        /// @param index The waypoint to select.
        /// @remarks This will not raise the on_waypoint_selected event.
        void select_waypoint(const Waypoint& waypoint);

        /// Set the current route.
        /// @param route The new route.
        void set_route(Route* route);

        /// Set whether the compass is visible.
        void set_show_compass(bool value);

        /// Set whether the minimap is visible.
        void set_show_minimap(bool value);

        /// Set whether the route is visible.
        void set_show_route(bool value);

        /// Set whether the selection is visible.
        void set_show_selection(bool value);

        /// Set whether the tools are visible.
        void set_show_tools(bool value);

        /// Set whether the tooltip is visible.
        void set_show_tooltip(bool value);

        /// Set whether the ui is visible.
        void set_show_ui(bool value);

        bool ui_input_active() const;
    private:
        void initialise_input();
        void toggle_highlight();
        void update_camera();
        void render_scene();
        bool should_pick() const;
        const ICamera& current_camera() const;
        ICamera& current_camera();
        void set_camera_mode(CameraMode camera_mode);
        void set_camera_projection_mode(ProjectionMode projection_mode);
        void set_alternate_mode(bool enabled);
        void toggle_alternate_mode();
        void set_alternate_group(uint32_t group, bool enabled);
        bool alternate_group(uint32_t group) const;
        // Tell things that need to be resized that they should resize.
        void resize_elements();
        // Set up keyboard and mouse input for the camera.
        void setup_camera_input();
        void set_show_triggers(bool show);
        void toggle_show_triggers();
        void set_show_hidden_geometry(bool show);
        void toggle_show_hidden_geometry();
        void set_show_water(bool show);
        void set_show_wireframe(bool show);
        uint32_t room_from_pick(const PickResult& pick) const;
        void add_recent_orbit(const PickResult& pick);
        void select_previous_orbit();
        void select_next_orbit();
        void select_pick(const PickResult& pick);

        void register_lua();
        void apply_acceleration_settings();

        graphics::Device& _device;
        Window _window;
        Shortcuts& _shortcuts;
        std::unique_ptr<graphics::DeviceWindow> _main_window;
        ILevel* _level{ nullptr };
        Timer _timer;
        OrbitCamera _camera;
        FreeCamera _free_camera;
        input::Keyboard _keyboard;
        std::unique_ptr<input::IMouse> _mouse;
        std::unique_ptr<IViewerUI> _ui;
        CameraMode _camera_mode{ CameraMode::Orbit };
        CameraInput _camera_input;
        UserSettings _settings;
        std::unique_ptr<IPicking> _picking;
        PickResult _current_pick;
        WindowResizer _window_resizer;
        TokenStore _token_store;
        AlternateGroupToggler _alternate_group_toggler;
        DirectX::SimpleMath::Vector3 _target;
        bool _show_selection{ true };
        SectorHighlight _sector_highlight;
        MenuDetector _menu_detector;

        // Tools:
        Tool _active_tool{ Tool::None };
        std::unique_ptr<Measure> _measure;
        std::unique_ptr<Compass> _compass;
        std::optional<Compass::Axis> _compass_axis;

        // Temporary route objects.
        PickResult _context_pick;
        Route* _route;
        bool _show_route{ true };

        /// Was the room just changed due to an alternate group or flip being performed?
        bool _was_alternate_select{ false };

        std::unique_ptr<graphics::RenderTarget> _scene_target;
        std::unique_ptr<graphics::Sprite> _scene_sprite;
        bool _scene_changed{ true };
        bool _mouse_changed{ true };
        bool _ui_changed{ true };

        std::vector<PickResult> _recent_orbits;
        std::size_t _recent_orbit_index{ 0u };
    };
}


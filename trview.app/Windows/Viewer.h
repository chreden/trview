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
#include <trview.app/Windows/IViewer.h>

namespace trview
{
    namespace graphics
    {
        class DeviceWindow;
    }

    /// Class that coordinates all the parts of the application.
    class Viewer : public IViewer
    {
    public:
        /// Create a new viewer.
        /// @param window The window that the viewer should use.
        explicit Viewer(
            const Window& window, 
            const std::shared_ptr<graphics::IDevice>& device,
            std::unique_ptr<IViewerUI> ui, 
            std::unique_ptr<IPicking> picking,
            std::unique_ptr<input::IMouse> mouse,
            const std::shared_ptr<IShortcuts>& shortcuts,
            const std::shared_ptr<IRoute> route,
            const graphics::ISprite::SpriteSource& sprite_source,
            std::unique_ptr<ICompass> compass);

        /// Destructor for the viewer.
        virtual ~Viewer() = default;

        /// Render the viewer.
        virtual void render() override;

        /// Attempt to open the specified level file.
        /// @param filename The level file to open.
        virtual void open(ILevel* level) override;

        virtual void set_settings(const UserSettings& settings) override;

        /// Select the specified item.
        /// @param item The item to select.
        /// @remarks This will not raise the on_item_selected event.
        virtual void select_item(const Item& item) override;

        /// Select the specified room.
        /// @param room The room to select.
        /// @remarks This will not raise the on_room_selected event.
        virtual void select_room(uint32_t room) override;

        /// Select the specified trigger.
        /// @param trigger The trigger to select.
        /// @remarks This will not raise the on_trigger_selected event.
        virtual void select_trigger(const Trigger* const trigger) override;

        /// Select the specified waypoint
        /// @param index The waypoint to select.
        /// @remarks This will not raise the on_waypoint_selected event.
        virtual void select_waypoint(const Waypoint& waypoint) override;

        /// Set the current route.
        /// @param route The new route.
        virtual void set_route(const std::shared_ptr<IRoute>& route) override;

        /// Set whether the compass is visible.
        virtual void set_show_compass(bool value) override;

        /// Set whether the minimap is visible.
        virtual void set_show_minimap(bool value) override;

        /// Set whether the route is visible.
        virtual void set_show_route(bool value) override;

        /// Set whether the selection is visible.
        virtual void set_show_selection(bool value) override;

        /// Set whether the tools are visible.
        virtual void set_show_tools(bool value) override;

        /// Set whether the tooltip is visible.
        virtual void set_show_tooltip(bool value) override;

        /// Set whether the ui is visible.
        virtual void set_show_ui(bool value) override;

        virtual bool ui_input_active() const override;
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

        const std::shared_ptr<graphics::IDevice> _device;
        Window _window;
        const std::shared_ptr<IShortcuts>& _shortcuts;
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
        std::unique_ptr<ICompass> _compass;
        std::optional<Compass::Axis> _compass_axis;

        // Temporary route objects.
        PickResult _context_pick;
        std::shared_ptr<IRoute> _route;
        bool _show_route{ true };

        /// Was the room just changed due to an alternate group or flip being performed?
        bool _was_alternate_select{ false };

        std::unique_ptr<graphics::RenderTarget> _scene_target;
        std::unique_ptr<graphics::ISprite> _scene_sprite;
        bool _scene_changed{ true };
        bool _mouse_changed{ true };
        bool _ui_changed{ true };

        std::vector<PickResult> _recent_orbits;
        std::size_t _recent_orbit_index{ 0u };
    };
}


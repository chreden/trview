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
#include <trview.graphics/IDevice.h>
#include <trview.input/Keyboard.h>
#include <trview.input/IMouse.h>
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
#include <trview.app/Graphics/ISectorHighlight.h>
#include <trview.app/UI/IViewerUI.h>
#include <trview.app/Elements/ITypeNameLookup.h>
#include <trview.app/Menus/MenuDetector.h>
#include <trview.app/Lua/Lua.h>
#include <trview.common/Windows/Shortcuts.h>
#include <trview.graphics/IDeviceWindow.h>
#include <trview.app/Windows/IViewer.h>
#include <trview.common/Windows/IClipboard.h>

namespace trview
{
    namespace graphics
    {
        class DeviceWindow;
    }

    /// Class that coordinates all the parts of the application.
    class Viewer : public IViewer, public MessageHandler
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
            const graphics::ISprite::Source& sprite_source,
            std::unique_ptr<ICompass> compass,
            std::unique_ptr<IMeasure> measure,
            const graphics::IRenderTarget::SizeSource& render_target_source,
            const graphics::IDeviceWindow::Source& device_window_source,
            std::unique_ptr<ISectorHighlight> sector_highlight,
            const std::shared_ptr<IClipboard>& clipboard);
        virtual ~Viewer() = default;
        virtual CameraMode camera_mode() const override;
        virtual void render() override;
        virtual void render_ui() override;
        virtual void present(bool vsync) override;
        virtual void open(ILevel* level, ILevel::OpenMode open_mode) override;
        virtual void set_settings(const UserSettings& settings) override;
        virtual void select_item(const Item& item) override;
        virtual void select_room(uint32_t room_number) override;
        virtual void select_trigger(const std::weak_ptr<ITrigger>& trigger) override;
        virtual void select_waypoint(const IWaypoint& waypoint) override;
        virtual void set_camera_mode(CameraMode camera_mode) override;
        virtual void set_route(const std::shared_ptr<IRoute>& route) override;
        virtual void set_show_compass(bool value) override;
        virtual void set_show_minimap(bool value) override;
        virtual void set_show_route(bool value) override;
        virtual void set_show_selection(bool value) override;
        virtual void set_show_tools(bool value) override;
        virtual void set_show_tooltip(bool value) override;
        virtual void set_show_ui(bool value) override;
        virtual bool ui_input_active() const override;
        virtual void select_light(const std::weak_ptr<ILight>& light) override;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        const ICamera& current_camera() const;
        ICamera& current_camera();
        virtual DirectX::SimpleMath::Vector3 target() const override;
        virtual void set_target(const DirectX::SimpleMath::Vector3& target) override;
        virtual void select_sector(const std::weak_ptr<ISector>& sector) override;
        virtual void set_scene_changed() override;
        virtual void select_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) override;
    private:
        void initialise_input();
        void toggle_highlight();
        void update_camera();
        void render_scene();
        bool should_pick() const;
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
        void set_show_items(bool show);
        void toggle_show_items();
        void set_show_geometry(bool show);
        void toggle_show_geometry();
        void toggle_show_lights();
        void set_show_water(bool show);
        void set_show_wireframe(bool show);
        void set_show_bounding_boxes(bool show);
        void set_show_lights(bool show);
        uint32_t room_from_pick(const PickResult& pick) const;
        void add_recent_orbit(const PickResult& pick);
        void select_previous_orbit();
        void select_next_orbit();
        void select_pick(const PickResult& pick);
        void set_show_rooms(bool show);
        void register_lua();
        void apply_camera_settings();
        void set_sector_highlight(const std::shared_ptr<ISector>& sector);
        void set_show_camera_sinks(bool show);
        void toggle_show_camera_sinks();

        const std::shared_ptr<graphics::IDevice> _device;
        const std::shared_ptr<IShortcuts>& _shortcuts;
        std::unique_ptr<graphics::IDeviceWindow> _main_window;
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
        std::unique_ptr<ISectorHighlight> _sector_highlight;
        MenuDetector _menu_detector;

        // Tools:
        Tool _active_tool{ Tool::None };
        std::unique_ptr<IMeasure> _measure;
        std::unique_ptr<ICompass> _compass;
        std::optional<Compass::Axis> _compass_axis;

        // Temporary route objects.
        PickResult _context_pick;
        std::shared_ptr<IRoute> _route;
        bool _show_route{ true };

        /// Was the room just changed due to an alternate group or flip being performed?
        bool _was_alternate_select{ false };

        graphics::IRenderTarget::SizeSource _render_target_source;
        std::unique_ptr<graphics::IRenderTarget> _scene_target;
        std::unique_ptr<graphics::ISprite> _scene_sprite;
        bool _scene_changed{ true };

        std::vector<PickResult> _recent_orbits;
        std::size_t _recent_orbit_index{ 0u };

        std::shared_ptr<IClipboard> _clipboard;
    };
}


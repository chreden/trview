#pragma once

#include <trview.common/TokenStore.h>
#include <trview.common/Window.h>
#include <trview.ui/Control.h>
#include <trview.ui.render/Renderer.h>
#include <trview.ui/Input.h>
#include <trview.input/Mouse.h>
#include <trview.input/Keyboard.h>
#include <trview.ui.render/MapRenderer.h>

#include <trview.app/Tools/Toolbar.h>
#include <trview.app/UI/LevelInfo.h>
#include <trview.app/UI/RoomNavigator.h>
#include <trview.app/UI/GoTo.h>
#include <trview.app/UI/SettingsWindow.h>
#include <trview.app/UI/CameraControls.h>
#include <trview.app/UI/CameraPosition.h>
#include <trview.app/Geometry/PickInfo.h>
#include <trview.app/Geometry/PickResult.h>
#include <trview.app/UI/ContextMenu.h>
#include <trview.app/Settings/UserSettings.h>
#include <trview.app/UI/Tooltip.h>
#include <trview.app/UI/ViewOptions.h>

namespace trview
{
    namespace graphics
    {
        struct IShaderStorage;
        class FontFactory;
    }

    enum class Tool
    {
        None,
        Measure
    };

    class ViewerUI final
    {
    public:
        explicit ViewerUI(const Window& window, 
            const graphics::Device& device, 
            const graphics::IShaderStorage& shader_storage,
            const graphics::FontFactory& font_factory,
            const ITextureStorage& texture_storage);

        /// Clear the highlighted minimap sector.
        void clear_minimap_highlight();

        /// Get the currently hovered minimap sector, if any.
        std::shared_ptr<Sector> current_minimap_sector() const;

        /// Get whether there is any text input currently active.
        bool is_input_active() const;

        /// Determiens if the cursor is over any ui element.
        /// @returns Whether the cursor is over an element.
        bool is_cursor_over() const;

        /// Event raised when the add waypoint button is pressed.
        Event<> on_add_waypoint;

        /// Event raised when the remove waypoint button is pressed.
        Event<> on_remove_waypoint;

        /// Event raised when the orbit here button is pressed.
        Event<> on_orbit;

        /// Event raised when an alternate group is toggled.
        Event<uint32_t, bool> on_alternate_group;

        /// Event raised when the camera mode is set.
        Event<CameraMode> on_camera_mode;

        /// Event raised when the camera projection mode is set.
        Event<ProjectionMode> on_camera_projection_mode;

        /// Event raised when the camera is reset.
        Event<> on_camera_reset;

        /// Event raised when the camera sensitivity is changed.
        Event<float> on_camera_sensitivity;

        /// Event raised when the camera movement speed is changed.
        Event<float> on_camera_movement_speed;

        /// Event raised when the depth level changes.
        Event<int32_t> on_depth_level_changed;

        /// Event raised when the depth setting is changed.
        Event<bool> on_depth;

        /// Event raised when the flip settings is changed.
        Event<bool> on_flip;

        /// Event raised when the higlight setting is changed.
        Event<bool> on_highlight;

        /// Event raised when a minimap sector is hovered over.
        Event<std::shared_ptr<Sector>> on_sector_hover;

        /// Event raised when an item is selected.
        Event<uint32_t> on_select_item;

        /// Event raised when a room is selected.
        Event<int32_t> on_select_room;

        /// Event raised when the user settings are changed.
        Event<UserSettings> on_settings;

        /// Event raised when the hidden geometry setting is changed.
        Event<bool> on_show_hidden_geometry;

        /// Event raised when the show triggers setting is changed.
        Event<bool> on_show_triggers;

        /// Event raised when the show water setting is changed.
        Event<bool> on_show_water;

        /// Event raised when a tool is selected.
        Event<Tool> on_tool_selected;

        /// Event raised when something in the ui has changed.
        Event<> on_ui_changed;

        /// Event raised when user edits camera position.
        Event<DirectX::SimpleMath::Vector3> on_camera_position;

        /// Render the UI.
        /// @param device The device to use to render the UI.
        void render(const graphics::Device& device);

        /// Set whether an alternate group is enabled.
        /// @param value The group to change.
        /// @param enabled Whether the group is enabled.
        void set_alternate_group(uint32_t value, bool enabled);

        /// Set the alternate groups for the level.
        /// @param groups The alternate groups for the level.
        void set_alternate_groups(const std::set<uint32_t>& groups);

        /// Set the camera movement speed.
        /// @param value The camera movement speed.
        void set_camera_movement_speed(float value);

        /// Set the current camera position.
        /// @param position The camera position.
        void set_camera_position(const DirectX::SimpleMath::Vector3& position);

        /// Set the sensitivity of the camera.
        /// @param value The camera sensitivity.
        void set_camera_sensitivity(float value);

        /// Set the camera mode.
        /// @param mode The current camera mode.
        void set_camera_mode(CameraMode mode);

        /// Set the camera projection mode.
        /// @param mode The current camera projection mode.
        void set_camera_projection_mode(ProjectionMode mode);

        /// Set whether depth is enabled.
        /// @param value Whether depth is enabled.
        void set_depth_enabled(bool value);

        /// Set the level of depth.
        /// @param value The depth level.
        void set_depth_level(int32_t value);

        /// Set the current flip state.
        /// @param value The flip state.
        void set_flip(bool value);

        /// Set whether there are any flipmaps in the level.
        /// @param value Whether there are any flipmaps.
        void set_flip_enabled(bool value);

        /// Set whether highlight is enabled.
        /// @param value Whether highlight is enabled.
        void set_highlight(bool value);

        /// Set the size of the host window.
        void set_host_size(const Size& size);

        /// Set the level name and version.
        /// @param name The file.
        /// @param version The version of the level.
        void set_level(const std::string& name, trlevel::LevelVersion version);

        /// Set the maximum number of rooms in the level.
        /// @param rooms The number of rooms that are in the level.
        void set_max_rooms(uint32_t rooms);

        /// Set the current measure distance to display on the label.
        /// @param distance The distance to measure.
        void set_measure_distance(float distance);

        /// Set the position of the measure label.
        /// @param position The position of the label.
        void set_measure_position(const Point& position);

        /// Set which square is highlighted on the minimap.
        /// @param x The x coordinate.
        /// @param z The z coordinate.
        void set_minimap_highlight(uint16_t x, uint16_t z);

        /// Set the current pick result.
        /// @param info The parameters for the pick.
        /// @param pick_result The pick result.
        void set_pick(const PickInfo& info, const PickResult& pick_result);

        /// Set whether the user can click the remove waypoint button.
        /// "param value Whether the button is enabled.
        void set_remove_waypoint_enabled(bool value);

        /// Set the selected room.
        /// @param room The selected room.
        void set_selected_room(Room* room);

        /// Set the user settings.
        /// @param settings The user settings.
        void set_settings(const UserSettings& settings);

        /// Set whether the context menu is visible.
        /// "param value Whether the context menu is visible.
        void set_show_context_menu(bool value);

        /// Set whether hidden geometry is visible.
        /// @param value Whether hidden geometry is visible.
        void set_show_hidden_geometry(bool value);

        /// Set whether to show the measure label.
        /// @param value Whether to show the measure label.
        void set_show_measure(bool value);

        /// Set whether to show the minimap.
        /// @param value Whether to show the minimap.
        void set_show_minimap(bool value);

        /// Set whether to show the tooltip.
        /// @param value Whether to show the tooltip.
        void set_show_tooltip(bool value);

        /// Set whether triggers are visible.
        /// @param value Whether triggers are visible.
        void set_show_triggers(bool value);

        /// Set whether water is visible.
        /// @param value Whether water is visible.
        void set_show_water(bool value);

        /// Set whether the level uses alternate groups.
        /// @param value Whether alternate groups are used.
        void set_use_alternate_groups(bool value);

        /// Set whether the UI is visible.
        /// @param value Whether the UI is visible.
        void set_visible(bool value);

        /// Get whether hidden geometry is visible.
        bool show_hidden_geometry() const;

        /// Get whether triggers are visible.
        bool show_triggers() const;

        /// Get whether water is visible.
        bool show_water() const;

        /// Toggle the visibility of the settings window.
        void toggle_settings_visibility();
    private:
        void generate_tool_window(const ITextureStorage& texture_storage);
        void initialise_camera_controls(ui::Control& parent);
        void register_change_detection(ui::Control* control);

        TokenStore _token_store;
        input::Mouse _mouse;
        input::Keyboard _keyboard;
        Window _window;
        UserSettings _settings;
        std::unique_ptr<ui::Control> _control;
        std::unique_ptr<ui::render::Renderer> _ui_renderer;
        std::unique_ptr<ui::Input> _ui_input;
        std::unique_ptr<ContextMenu> _context_menu;
        std::unique_ptr<GoTo> _go_to;
        std::unique_ptr<RoomNavigator> _room_navigator;
        std::unique_ptr<ViewOptions> _view_options;
        std::unique_ptr<Toolbar> _toolbar;
        std::unique_ptr<LevelInfo> _level_info;
        std::unique_ptr<SettingsWindow> _settings_window;
        std::unique_ptr<CameraControls> _camera_controls;
        std::unique_ptr<CameraPosition> _camera_position;
        std::unique_ptr<ui::render::MapRenderer> _map_renderer;
        std::unique_ptr<Tooltip> _map_tooltip;
        std::unique_ptr<Tooltip> _tooltip;
        ui::Label* _measure;
        bool _show_tooltip{ true };
    };
}

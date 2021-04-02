#pragma once

#include <memory>

#include <trview.app/Camera/CameraMode.h>
#include <trview.app/Elements/Sector.h>
#include <trview.app/Elements/Room.h>
#include <trview.app/Geometry/PickInfo.h>
#include <trview.app/Settings/UserSettings.h>

#include <trview.common/Event.h>

namespace trview
{
    enum class Tool
    {
        None,
        Measure
    };

    struct IViewerUI
    {
        virtual ~IViewerUI() = 0;

        /// Clear the highlighted minimap sector.
        virtual void clear_minimap_highlight() = 0;

        /// Get the currently hovered minimap sector, if any.
        virtual std::shared_ptr<Sector> current_minimap_sector() const = 0;

        /// Get whether there is any text input currently active.
        virtual bool is_input_active() const = 0;

        /// Determiens if the cursor is over any ui element.
        /// @returns Whether the cursor is over an element.
        virtual bool is_cursor_over() const = 0;

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

        /// Event raised when the depth level changes.
        Event<int32_t> on_depth_level_changed;

        /// Event raised when the depth setting is changed.
        Event<bool> on_depth;

        /// Event raised when the flip settings is changed.
        Event<bool> on_flip;

        /// Event raised when the hide button is clicked.
        Event<> on_hide;

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

        /// Event raised when the show wireframe setting is changed.
        Event<bool> on_show_wireframe;

        /// Event raised when a tool is selected.
        Event<Tool> on_tool_selected;

        /// Event raised when something in the ui has changed.
        Event<> on_ui_changed;

        /// Event raised when user edits camera position.
        Event<DirectX::SimpleMath::Vector3> on_camera_position;

        /// Event raised when user enters a command.
        Event<std::wstring> on_command;

        /// Render the UI.
        virtual void render() = 0;

        /// Set whether an alternate group is enabled.
        /// @param value The group to change.
        /// @param enabled Whether the group is enabled.
        virtual void set_alternate_group(uint32_t value, bool enabled) = 0;

        /// Set the alternate groups for the level.
        /// @param groups The alternate groups for the level.
        virtual void set_alternate_groups(const std::set<uint32_t>& groups) = 0;

        /// Set the current camera position.
        /// @param position The camera position.
        virtual void set_camera_position(const DirectX::SimpleMath::Vector3& position) = 0;

        /// Set the camera mode.
        /// @param mode The current camera mode.
        virtual void set_camera_mode(CameraMode mode) = 0;

        /// Set the camera projection mode.
        /// @param mode The current camera projection mode.
        virtual void set_camera_projection_mode(ProjectionMode mode) = 0;

        /// Set whether depth is enabled.
        /// @param value Whether depth is enabled.
        virtual void set_depth_enabled(bool value) = 0;

        /// Set the level of depth.
        /// @param value The depth level.
        virtual void set_depth_level(int32_t value) = 0;

        /// Set the current flip state.
        /// @param value The flip state.
        virtual void set_flip(bool value) = 0;

        /// Set whether there are any flipmaps in the level.
        /// @param value Whether there are any flipmaps.
        virtual void set_flip_enabled(bool value) = 0;

        /// Set whether the hide button on the context menu is enabled.
        /// @param value Whether the hide button is enabled.
        virtual void set_hide_enabled(bool value) = 0;

        /// Set whether highlight is enabled.
        /// @param value Whether highlight is enabled.
        virtual void set_highlight(bool value) = 0;

        /// Set the size of the host window.
        virtual void set_host_size(const Size& size) = 0;

        /// Set the level name and version.
        /// @param name The file.
        /// @param version The version of the level.
        virtual void set_level(const std::string& name, trlevel::LevelVersion version) = 0;

        /// Set the maximum number of rooms in the level.
        /// @param rooms The number of rooms that are in the level.
        virtual void set_max_rooms(uint32_t rooms) = 0;

        /// Set the current measure distance to display on the label.
        /// @param distance The distance to measure.
        virtual void set_measure_distance(float distance) = 0;

        /// Set the position of the measure label.
        /// @param position The position of the label.
        virtual void set_measure_position(const Point& position) = 0;

        /// Set which square is highlighted on the minimap.
        /// @param x The x coordinate.
        /// @param z The z coordinate.
        virtual void set_minimap_highlight(uint16_t x, uint16_t z) = 0;

        /// Set the current pick result.
        /// @param info The parameters for the pick.
        /// @param pick_result The pick result.
        virtual void set_pick(const PickInfo& info, const PickResult& pick_result) = 0;

        /// Set whether the user can click the remove waypoint button.
        /// "param value Whether the button is enabled.
        virtual void set_remove_waypoint_enabled(bool value) = 0;

        /// Set the selected room.
        /// @param room The selected room.
        virtual void set_selected_room(Room* room) = 0;

        /// Set the user settings.
        /// @param settings The user settings.
        virtual void set_settings(const UserSettings& settings) = 0;

        /// Set whether the context menu is visible.
        /// "param value Whether the context menu is visible.
        virtual void set_show_context_menu(bool value) = 0;

        /// Set whether hidden geometry is visible.
        /// @param value Whether hidden geometry is visible.
        virtual void set_show_hidden_geometry(bool value) = 0;

        /// Set whether to show the measure label.
        /// @param value Whether to show the measure label.
        virtual void set_show_measure(bool value) = 0;

        /// Set whether to show the minimap.
        /// @param value Whether to show the minimap.
        virtual void set_show_minimap(bool value) = 0;

        /// Set whether to show the tooltip.
        /// @param value Whether to show the tooltip.
        virtual void set_show_tooltip(bool value) = 0;

        /// Set whether triggers are visible.
        /// @param value Whether triggers are visible.
        virtual void set_show_triggers(bool value) = 0;

        /// Set whether water is visible.
        /// @param value Whether water is visible.
        virtual void set_show_water(bool value) = 0;

        /// Set whether wireframe is visible.
        /// @param value Whether wireframe is visible.
        virtual void set_show_wireframe(bool value) = 0;

        /// Set whether the level uses alternate groups.
        /// @param value Whether alternate groups are used.
        virtual void set_use_alternate_groups(bool value) = 0;

        /// Set whether the UI is visible.
        /// @param value Whether the UI is visible.
        virtual void set_visible(bool value) = 0;

        /// Get whether hidden geometry is visible.
        virtual bool show_hidden_geometry() const = 0;

        /// Get whether triggers are visible.
        virtual bool show_triggers() const = 0;

        /// Get whether water is visible.
        virtual bool show_water() const = 0;

        /// Get whether wireframe is visible.
        virtual bool show_wireframe() const = 0;

        /// Get whether the context menu is visible.
        virtual bool show_context_menu() const = 0;

        /// Toggle the visibility of the settings window.
        virtual void toggle_settings_visibility() = 0;

        /// <summary>
        /// Write the text to the console.
        /// </summary>
        /// <param name="text">The text to write.</param>
        virtual void print_console(const std::wstring& text) = 0;

        virtual void initialise_input() = 0;
    };
}

#pragma once

#include <memory>

#include <trview.app/Camera/CameraMode.h>
#include <trview.app/Elements/ISector.h>
#include <trview.app/Geometry/PickInfo.h>
#include <trview.app/Settings/UserSettings.h>
#include "IContextMenu.h"

#include <trview.common/Event.h>


namespace trview
{
    struct IRoom;
    struct IItem;

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
        virtual std::shared_ptr<ISector> current_minimap_sector() const = 0;

        /// Get whether there is any text input currently active.
        virtual bool is_input_active() const = 0;

        /// Determiens if the cursor is over any ui element.
        /// @returns Whether the cursor is over an element.
        virtual bool is_cursor_over() const = 0;

        /// Event raised when the add waypoint button is pressed.
        Event<> on_add_waypoint;

        /// Event raised when the add mid waypoint button is pressed.
        Event<> on_add_mid_waypoint;

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

        /// <summary>
        /// Event raised when a scalar value has changed.
        /// </summary>
        Event<std::string, int32_t> on_scalar_changed;

        /// Event raised when the hide button is clicked.
        Event<> on_hide;

        /// Event raised when a minimap sector is hovered over.
        Event<std::shared_ptr<ISector>> on_sector_hover;

        /// Event raised when an item is selected.
        Event<std::weak_ptr<IItem>> on_select_item;

        /// Event raised when a room is selected.
        Event<std::weak_ptr<IRoom>> on_select_room;

        /// Event raised when the user settings are changed.
        Event<UserSettings> on_settings;

        /// Event raised when a tool is selected.
        Event<Tool> on_tool_selected;

        /// Event raised when user edits camera position.
        Event<DirectX::SimpleMath::Vector3> on_camera_position;

        /// <summary>
        /// Event raised when the camera rotation is changed.
        /// </summary>
        Event<float, float> on_camera_rotation;

        /// <summary>
        /// Event raised when the user changes a toggle.
        /// </summary>
        Event<std::string, bool> on_toggle_changed;

        /// <summary>
        /// Event raised when the user has clicked a copy button.
        /// </summary>
        Event<IContextMenu::CopyType> on_copy;

        Event<std::weak_ptr<ITrigger>> on_select_trigger;

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

        /// <summary>
        /// Set the camera rotation for the rotation display.
        /// </summary>
        /// <param name="yaw">The yaw in radians.</param>
        /// <param name="pitch">The pitch in radians.</param>
        virtual void set_camera_rotation(float yaw, float pitch) = 0;

        /// Set the camera mode.
        /// @param mode The current camera mode.
        virtual void set_camera_mode(CameraMode mode) = 0;

        /// Set the camera projection mode.
        /// @param mode The current camera projection mode.
        virtual void set_camera_projection_mode(ProjectionMode mode) = 0;

        /// Set whether there are any flipmaps in the level.
        /// @param value Whether there are any flipmaps.
        virtual void set_flip_enabled(bool value) = 0;

        /// Set whether the hide button on the context menu is enabled.
        /// @param value Whether the hide button is enabled.
        virtual void set_hide_enabled(bool value) = 0;

        /// Set the size of the host window.
        virtual void set_host_size(const Size& size) = 0;

        /// Set the level name and version.
        /// @param name The file.
        /// @param version The version of the level.
        virtual void set_level(const std::string& name, const std::weak_ptr<ILevel>& level) = 0;

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
        /// @param pick_result The pick result.
        virtual void set_pick(const PickResult& pick_result) = 0;

        /// Set whether the user can click the remove waypoint button.
        /// "param value Whether the button is enabled.
        virtual void set_remove_waypoint_enabled(bool value) = 0;

        /// <summary>
        /// Set the currently selected item index.
        /// </summary>
        /// <param name="item">The selected item number.</param>
        virtual void set_selected_item(uint32_t index) = 0;

        /// Set the selected room.
        /// @param room The selected room.
        virtual void set_selected_room(const std::shared_ptr<IRoom>& room) = 0;

        /// Set the user settings.
        /// @param settings The user settings.
        virtual void set_settings(const UserSettings& settings) = 0;

        /// Set whether the context menu is visible.
        /// "param value Whether the context menu is visible.
        virtual void set_show_context_menu(bool value) = 0;

        /// Set whether to show the measure label.
        /// @param value Whether to show the measure label.
        virtual void set_show_measure(bool value) = 0;

        /// Set whether to show the minimap.
        /// @param value Whether to show the minimap.
        virtual void set_show_minimap(bool value) = 0;

        /// Set whether to show the tooltip.
        /// @param value Whether to show the tooltip.
        virtual void set_show_tooltip(bool value) = 0;

        /// Set whether the level uses alternate groups.
        /// @param value Whether alternate groups are used.
        virtual void set_use_alternate_groups(bool value) = 0;

        /// Set whether the UI is visible.
        /// @param value Whether the UI is visible.
        virtual void set_visible(bool value) = 0;

        /// Get whether the context menu is visible.
        virtual bool show_context_menu() const = 0;

        /// Toggle the visibility of the settings window.
        virtual void toggle_settings_visibility() = 0;

        /// <summary>
        /// Set whether the mid waypoint button is enabled.
        /// </summary>
        /// <param name="value">Whether the button is enabled.</param>
        virtual void set_mid_waypoint_enabled(bool value) = 0;
        /// <summary>
        /// Set the value of a scalar setting.
        /// </summary>
        /// <param name="name">The name of the setting.</param>
        /// <param name="value">The value to set.</param>
        virtual void set_scalar(const std::string& name, int32_t value) = 0;
        /// <summary>
        /// Set the value of a toggle variable.
        /// </summary>
        /// <param name="name">The name of the variable.</param>
        /// <param name="value">The new state.</param>
        virtual void set_toggle(const std::string& name, bool value) = 0;
        /// <summary>
        /// Get the value of a toggle variable.
        /// </summary>
        /// <param name="name">The name of the variable.</param>
        /// <returns>The state of the variable.</returns>
        virtual bool toggle(const std::string& name) const = 0;
        /// <summary>
        /// Set the triggers that trigger the item in the context menu.
        /// </summary>
        /// <param name="triggers"></param>
        virtual void set_triggered_by(const std::vector<std::weak_ptr<ITrigger>>& triggers) = 0;

        virtual void set_route(const std::weak_ptr<IRoute>& route) = 0;
        /// <summary>
        /// Clear the currently loaded room from the minimap.
        /// </summary>
        virtual void unload_minimap() = 0;
    };
}

#pragma once

#include <trview.common/Event.h>
#include "../UI/MapColours.h"

namespace trview
{
    struct ISettingsWindow
    {
        virtual ~ISettingsWindow() = 0;
        /// <summary>
        /// Event raised when the vsync settings has been changed. The new vsync setting is passed as the parameter.
        /// </summary>
        Event<bool> on_vsync;
        /// <summary>
        /// Event raised when the 'go to lara' setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<bool> on_go_to_lara;
        /// <summary>
        /// Event raised when the inverted map controls setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<bool> on_invert_map_controls;
        /// <summary>
        /// Event raised when the 'items window at startup' setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<bool> on_items_startup;
        /// <summary>
        /// Event raised when the movement speed has been changed. The new movement speed value is passed as a parameter when the event is raised.
        /// </summary>
        /// <remarks>This event is not raised by the set_movement_speed function.</remarks>
        Event<float> on_movement_speed_changed;
        /// <summary>
        /// Event raised when the camera sensitivity has been changed by the user. The new sensitivity value is passed as a parameter when the event is raised.
        /// </summary>
        /// <remarks>This event is not raised by the set_sensitivity function.</remarks>
        Event<float> on_sensitivity_changed;
        /// <summary>
        /// Event raised when the 'triggers window at startup' setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<bool> on_triggers_startup;
        /// <summary>
        /// Event raised when the 'rooms window at startup' setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<bool> on_rooms_startup;
        /// <summary>
        /// Event raised when the 'Switch to orbit on selection' setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<bool> on_auto_orbit;
        /// <summary>
        /// Event raised when the 'Invert vertical pan' setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<bool> on_invert_vertical_pan;
        /// <summary>
        /// Event raised when the 'camera acceleration' setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<bool> on_camera_acceleration;
        /// <summary>
        /// Event raised when the 'camera acceleration rate' setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<float> on_camera_acceleration_rate;
        /// <summary>
        /// Event raised when the 'camera display degrees' setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<bool> on_camera_display_degrees;
        /// <summary>
        /// Event raised when the 'randomizer tools' setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<bool> on_randomizer_tools;
        /// <summary>
        /// Event raised when the 'max recent files' setting has been changed. The new setting is passed as the parameter.
        /// </summary>
        Event<uint32_t> on_max_recent_files;
        Event<Colour> on_background_colour;
        Event<MapColours> on_minimap_colours;

        Event<Colour> on_default_route_colour;
        Event<Colour> on_default_waypoint_colour;
        Event<bool> on_route_startup;
        Event<float> on_camera_fov;

        virtual void render() = 0;
        /// <summary>
        /// Set the new value of the vsync setting. This will not raise the on_vsync event.
        /// </summary>
        /// <param name="value">The new vsync setting.</param>
        virtual void set_vsync(bool value) = 0;
        /// <summary>
        /// Set the new value of the 'go to lara' setting. This will not raise the on_go_to_lara event.
        /// </summary>
        /// <param name="value">The new 'go to lara' setting.</param>
        virtual void set_go_to_lara(bool value) = 0;
        /// <summary>
        /// Set the new value of the inverted map controls setting. This will not raise the on_invert_map_controls event.
        /// </summary>
        /// <param name="value">The new inverted map controls setting.</param>
        virtual void set_invert_map_controls(bool value) = 0;
        /// <summary>
        /// Set the new value of the 'items window at startup' setting. This will not raise the on_items_startup event.
        /// </summary>
        /// <param name="value">The new 'items window at startup' setting.</param>
        virtual void set_items_startup(bool value) = 0;
        /// <summary>
        /// Set the new value of the 'triggers window at startup' setting. This will not raise the on_triggers_startup event.
        /// </summary>
        /// <param name="value">The new 'triggers window at startup' setting.</param>
        virtual void set_triggers_startup(bool value) = 0;
        /// <summary>
        /// Set the new value of the 'rooms window at startup' setting. This will not raise the on_rooms_startup event.
        /// </summary>
        /// <param name="value">The new 'rooms window at startup' setting.</param>
        virtual void set_rooms_startup(bool value) = 0;
        /// <summary>
        /// Set the new value of the 'Switch to orbit on selection' setting. This will not raise the on_auto_orbit event.
        /// </summary>
        /// <param name="value">The new 'Switch to orbit on selection' setting.</param>
        virtual void set_auto_orbit(bool value) = 0;
        /// <summary>
        /// Set the new value of the 'camera acceleration' setting. This will not raise the on_camera_acceleration event.
        /// </summary>
        /// <param name="value">The new 'camera acceleration' setting.</param>
        virtual void set_camera_acceleration(bool value) = 0;
        /// <summary>
        /// Set the new value of the 'camera acceleration rate' setting. This will not raise the on_camera_acceleration_rate event.
        /// </summary>
        /// <param name="value">The new 'camera acceleration rate' setting.</param>
        virtual void set_camera_acceleration_rate(float value) = 0;
        /// <summary>
        /// Set the new value of the 'default route colour' setting. This will not raise the on_default_route_colour event.
        /// </summary>
        /// <param name="colour">The new 'default route colour' setting.</param>
        virtual void set_default_route_colour(const Colour& colour) = 0;
        /// <summary>
        /// Set the new value of the 'default waypoint colour' setting. This will not raise the on_default_waypoint_colour event.
        /// </summary>
        /// <param name="colour">The new 'default waypoint colour' setting.</param>
        virtual void set_default_waypoint_colour(const Colour& colour) = 0;
        /// <summary>
        /// Set the movement speed slider to specified value.
        /// </summary>
        /// <param name="value">The movement speed between 0 and 1.</param>
        /// <remarks>This will not raise the on_movement_speed_changed event.</remarks>
        virtual void set_movement_speed(float value) = 0;
        /// <summary>
        /// Set the sensitivity slider to the specified value.
        /// </summary>
        /// <param name="value">The sensitivity value between 0 and 1.</param>
        /// <remarks>This will not raise the on_sensitivity_changed event.</remarks>
        virtual void set_sensitivity(float value) = 0;
        /// <summary>
        /// Set the value of invert vertical pan.
        /// </summary>
        /// <remarks>This will not raise the on_invert_vertical_pan event.</remarks>
        /// <param name="value">The new setting value.</param>
        virtual void set_invert_vertical_pan(bool value) = 0;
        /// <summary>
        /// Set the value of camera display degrees.
        /// </summary>
        /// <remarks>This will not raise the on_camera_display_degrees event.</remarks>
        /// <param name="value">The new setting value.</param>
        virtual void set_camera_display_degrees(bool value) = 0;
        /// <summary>
        /// Set the value of randomizer tools.
        /// </summary>
        /// <remarks>This will not raise the on_randomimzer_tools event.</remarks>
        /// <param name="value">The new setting value.</param>
        virtual void set_randomizer_tools(bool value) = 0;
        /// <summary>
        /// Set the maximum number of recent files.
        /// </summary>
        /// <param name="value">The new setting value.</param>
        virtual void set_max_recent_files(uint32_t value) = 0;
        virtual void set_background_colour(const Colour& colour) = 0;
        virtual void set_map_colours(const MapColours& colours) = 0;
        virtual void set_route_startup(bool value) = 0;
        virtual void set_fov(float value) = 0;
        /// <summary>
        /// Toggle the visibility of the settings window.
        /// </summary>
        virtual void toggle_visibility() = 0;
    };
}

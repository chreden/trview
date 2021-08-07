#pragma once

#include <functional>
#include <cstdint>
#include <trview.common/Event.h>
#include <trview.ui/Control.h>
#include <trview.app/Graphics/ITextureStorage.h>

namespace trview
{
    struct IViewOptions
    {
        using Source = std::function<std::unique_ptr<IViewOptions>(ui::Control&, const ITextureStorage&)>;

        virtual ~IViewOptions() = 0;
        /// <summary>
        /// Event raised when an alternate group is toggled. The group number and the new state are passed as parameters.
        /// </summary>
        Event<uint32_t, bool> on_alternate_group;
        /// <summary>
        /// Event raised when the user has changed the depth of neighbour to display. The newly selected depth is passed when the event is raised.
        /// </summary>
        Event<int32_t> on_depth_changed;
        /// <summary>
        /// Event raised when the user has enabled or disabled neighbour mode. The boolean passed to when the event is raised indicates whether neighbours mode is enabled.
        /// </summary>
        /// <remarks>This event is not raised when the set_enabled function is called.</remarks>
        Event<bool> on_depth_enabled;
        /// <summary>
        /// Event raised when the user toggles the alternate mode. The boolean passed as a parameter when this event is raised indicates whether flip mode is enabled.
        /// </summary>
        /// <remarks>This event is not raised by the set_flip function.</remarks>
        Event<bool> on_flip;
        /// Event raised when the user toggles the highlight mode. The boolean passed as a parameter when this
        /// event is raised indicates whether highlight mode is enabled.
        /// @remarks This event is not raised by the set_highlight function.
        Event<bool> on_highlight;
        /// <summary>
        /// Event raised when the user toggles the hidden geometry visibility. The boolean passed as a parameter when this event is raised indicates whether hidden geomety is visible.
        /// </summary>
        /// <remarks>This event is not raised by the set_show_hidden_geometry function.</remarks>
        Event<bool> on_show_hidden_geometry;
        /// <summary>
        /// Event raised when the user toggles the trigger visibility. The boolean passed as a parameter when this event is raised indicates whether triggers are visible.
        /// </summary>
        /// <remarks>This event is not raised by the set_show_triggers function.</remarks>
        Event<bool> on_show_triggers;
        /// <summary>
        /// Event raised when the user toggles showing water. The boolean passed as a paramter when this event is raised indicates whether water colouring is visible.
        /// </summary>
        /// <remarks>This event is not raised by the set_show_water function.</remarks>
        Event<bool> on_show_water;
        /// <summary>
        /// Event raised when the user toggles wireframe mode. The boolean passed as a parameter when this event is raised indicates whether wireframe mode is used.
        /// </summary>
        /// <remarks>This event is not raised by the set_wireframe function.</remarks>
        Event<bool> on_show_wireframe;
        /// <summary>
        /// Event raised when the user toggles bounding boxes. The boolean passed as a parameter when this event is raised.
        /// </summary>
        /// <remarks>This event is not raised by the set_show_bounding_boxes function.</remarks>
        Event<bool> on_show_bounding_boxes;
        /// <summary>
        /// Set whether an alternate group is enabled. This will not raise the on_alternate_group event.
        /// </summary>
        /// <param name="value">The group to change.</param>
        /// <param name="enabled">Whether the group is enabled.</param>
        virtual void set_alternate_group(uint32_t value, bool enabled) = 0;
        /// <summary>
        /// Set the alternate groups that are in the level.
        /// </summary>
        /// <param name="groups">The groups in the level.</param>
        virtual void set_alternate_groups(const std::set<uint32_t>& groups) = 0;
        /// <summary>
        /// Set the value of the depth control. This will not raise the on_depth_changed event.
        /// </summary>
        /// <param name="value">The neighbour depth to use.</param>
        virtual void set_depth(int32_t value) = 0;
        /// <summary>
        /// Set whether neighbours are enabled. This will not raise the on_enabled_changed event.
        /// </summary>
        /// <param name="value">Whether neighbours are enabled.</param>
        virtual void set_depth_enabled(bool value) = 0;
        /// <summary>
        /// Set the current flip mode. This will not raise the on_flip event but will update the user interface appropriately.
        /// </summary>
        /// <param name="flip">The new flip mode.</param>
        virtual void set_flip(bool flip) = 0;
        /// <summary>
        /// Set whether the flip control is enabled or disabled.
        /// </summary>
        /// <param name="enabled">Whether the control is enabled.</param>
        virtual void set_flip_enabled(bool enabled) = 0;
        /// <summary>
        /// Set the current highlight mode. This will not raise the on_highlight event but will update the user interface appropriately.
        /// </summary>
        /// <param name="highlight">Whether the highlight mode is enabled or disabled.</param>
        virtual void set_highlight(bool highlight) = 0;
        /// <summary>
        /// Set whether hidden geometry is visible or not.
        /// </summary>
        /// <param name="show">Whether the hidden geometry is visible.</param>
        virtual void set_show_hidden_geometry(bool show) = 0;
        /// <summary>
        /// Set whether triggers are visible or not.
        /// </summary>
        /// <param name="show">Whether the triggers are visible.</param>
        virtual void set_show_triggers(bool show) = 0;
        /// <summary>
        /// Set whether water is visible or not.
        /// </summary>
        /// <param name="show">Whether water is visible.</param>
        virtual void set_show_water(bool show) = 0;
        /// <summary>
        /// Set whether wireframe mode is enabled.
        /// </summary>
        /// <param name="show">Whether to use wireframe.</param>
        virtual void set_show_wireframe(bool show) = 0;
        /// <summary>
        /// Set whether to use alternate groups method of flipmaps.
        /// </summary>
        /// <param name="value">Whether to use alternate groups or a single toggle.</param>
        virtual void set_use_alternate_groups(bool value) = 0;
        /// <summary>
        /// Set whether to show bounding boxes.
        /// </summary>
        /// <param name="value">Whether to show bouding boxes.</param>
        virtual void set_show_bounding_boxes(bool value) = 0;
        /// <summary>
        /// Get the current value of the show hidden geometry checkbox.
        /// </summary>
        /// <returns>The current value of the checkbox.</returns>
        virtual bool show_hidden_geometry() const = 0;
        /// <summary>
        /// Get the current value of the show triggers checkbox.
        /// </summary>
        /// <returns>The current value of the checkbox.</returns>
        virtual bool show_triggers() const = 0;
        /// <summary>
        /// Get the current value of the show water checkbox.
        /// </summary>
        /// <returns>The current value of the checkbox.</returns>
        virtual bool show_water() const = 0;
        /// <summary>
        /// Get the current value of the wireframe checkbox.
        /// </summary>
        /// <returns>The current value of the checkbox.</returns>
        virtual bool show_wireframe() const = 0;
        /// <summary>
        /// Get the current value of the bounding boxes checkbox.
        /// </summary>
        /// <returns>The current value of the checkbox.</returns>
        virtual bool show_bounding_boxes() const = 0;
    };
}

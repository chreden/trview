#pragma once

#include <functional>
#include <cstdint>
#include <trview.common/Event.h>
#include <trview.ui/Control.h>

namespace trview
{
    struct IViewOptions
    {
        using Source = std::function<std::unique_ptr<IViewOptions>(ui::Control&)>;

        virtual ~IViewOptions() = 0;
        /// <summary>
        /// Event raised when an alternate group is toggled. The group number and the new state are passed as parameters.
        /// </summary>
        Event<uint32_t, bool> on_alternate_group;
        /// <summary>
        /// Event raised when the user changes a scalar setting.
        /// </summary>
        Event<std::string, int32_t> on_scalar_changed;
        /// <summary>
        /// Event raised when the user changes a toggle.
        /// </summary>
        Event<std::string, bool> on_toggle_changed;
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
        /// Set whether to use alternate groups method of flipmaps.
        /// </summary>
        /// <param name="value">Whether to use alternate groups or a single toggle.</param>
        virtual void set_use_alternate_groups(bool value) = 0;
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
        /// Set whether the flip control is enabled or disabled.
        /// </summary>
        /// <param name="enabled">Whether the control is enabled.</param>
        virtual void set_flip_enabled(bool enabled) = 0;
    };
}

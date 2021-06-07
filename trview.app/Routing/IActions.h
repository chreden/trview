#pragma once

#include <vector>
#include <trlevel/LevelVersion.h>
#include "Action.h"

namespace trview
{
    /// <summary>
    /// Defines all actions that can performed in a route.
    /// </summary>
    struct IActions
    {
        virtual ~IActions() = 0;

        /// <summary>
        /// Get all actions that have an entry for a specific level version.
        /// </summary>
        /// <param name="version">The game version that the level is made for.</param>
        /// <returns>The compatible actions.</returns>
        virtual std::vector<Action> actions(trlevel::LevelVersion version) const = 0;

        /// <summary>
        /// Get the action with the specified name.
        /// </summary>
        /// <param name="name">The name of the action.</param>
        /// <returns>The action or an empty optional if not found.</returns>
        virtual std::optional<Action> action(const std::string& name) const = 0;
    };
}

#pragma once

#include <unordered_set>
#include <string>

namespace trview
{
    /// <summary>
    /// Information about a game type.
    /// </summary>
    struct TypeInfo final
    {
        /// <summary>
        /// Friendly name for the type.
        /// </summary>
        std::string name;
        /// <summary>
        /// Whether this is a pickup item - used for position adjustment.
        /// </summary>
        bool pickup{ false };
        /// <summary>
        /// Any named categories that this item belongs to.
        /// </summary>
        std::unordered_set<std::string> categories;
    };
}

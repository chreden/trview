export module trview.app:TypeInfo;

import std;

namespace trview
{
    /// <summary>
    /// Information about a game type.
    /// </summary>
    export struct TypeInfo final
    {
        /// <summary>
        /// Friendly name for the type.
        /// </summary>
        std::string name;
        /// <summary>
        /// Any named categories that this item belongs to.
        /// </summary>
        std::unordered_set<std::string> categories;
    };
}

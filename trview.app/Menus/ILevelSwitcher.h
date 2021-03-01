#pragma once

#include <string>
#include <trview.common/Event.h>

namespace trview
{
    struct ILevelSwitcher
    {
        virtual ~ILevelSwitcher() = 0;

        /// Open the specified file. This will populate the switcher menu.
        /// @param filename The file that was opened.
        virtual void open_file(const std::string& filename) = 0;

        /// Event raised when the user switches level. The opened level is passed as a parameter.
        Event<std::string> on_switch_level;
    };
}

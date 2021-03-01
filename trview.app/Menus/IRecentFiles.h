#pragma once

#include <string>
#include <trview.common/Event.h>

namespace trview
{
    struct IRecentFiles
    {
        virtual ~IRecentFiles() = 0;
        virtual void set_recent_files(const std::list<std::string>& files) = 0;

        /// Event raised when the user opens a file from the recent files list. The file that was opened
        /// is passed as a parameter.
        Event<std::string> on_file_open;
    };
}

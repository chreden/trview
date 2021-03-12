#pragma once

#include <trview.common/Event.h>
#include <string>

namespace trview
{
    struct IFileDropper
    {
        virtual ~IFileDropper() = 0;

        /// Event raised when the user drops a file on to the window. The file dropped is passed as a parameter.
        Event<std::string> on_file_dropped;
    };
}

#pragma once

#include <list>
#include <string>
#include <trview.common/Event.h>

namespace trview
{
    struct IFileMenu
    {
        virtual ~IFileMenu() = 0;
        /// <summary>
        /// Open the specified file. This will populate the switcher menu.
        /// </summary>
        /// <param name="filename">The file that was opened.</param>
        virtual void open_file(const std::string& filename) = 0;
        /// <summary>
        /// Set the list of recent files to display in the menu.
        /// </summary>
        /// <param name="files">The files to show.</param>
        virtual void set_recent_files(const std::list<std::string>& files) = 0;
        /// <summary>
        /// Event raised when the user opens a level. The opened level is passed as a parameter.
        /// </summary>
        Event<std::string> on_file_open;
        /// <summary>
        /// Event raised when the user wants to reload the level.
        /// </summary>
        Event<> on_reload;
    };
}


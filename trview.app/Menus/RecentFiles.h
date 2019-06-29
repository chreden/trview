/// @file RecentFiles.h
/// @brief When the user opens a file this will update a menu with the most recent files.

#pragma once

#include <string>
#include <list>
#include <vector>
#include <trview.common/Event.h>
#include <trview.common/MessageHandler.h>

namespace trview
{
    /// Updates a menu with the most recent files.
    class RecentFiles final : public MessageHandler
    {
    public:
        /// Create a new RecentFiles that will manage a menu on the window specified.
        /// @param window The window that has the recent files menu.
        explicit RecentFiles(const Window& window);

        /// Destructor for RecentFiles.
        virtual ~RecentFiles() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Event raised when the user opens a file from the recent files list. The file that was opened
        /// is passed as a parameter.
        Event<std::string> on_file_open;

        /// Set the current list of recent files.
        /// @param files The list of recent files.
        void set_recent_files(const std::list<std::string>& files);
    private:
        std::vector<std::string> _recent_files;
    };
}

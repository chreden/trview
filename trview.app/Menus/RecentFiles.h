/// @file RecentFiles.h
/// @brief When the user opens a file this will update a menu with the most recent files.

#pragma once

#include <list>
#include <vector>
#include <trview.common/MessageHandler.h>
#include "IRecentFiles.h"

namespace trview
{
    /// Updates a menu with the most recent files.
    class RecentFiles final : public IRecentFiles, public MessageHandler
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
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Set the current list of recent files.
        /// @param files The list of recent files.
        virtual void set_recent_files(const std::list<std::string>& files) override;
    private:
        std::vector<std::string> _recent_files;
    };
}

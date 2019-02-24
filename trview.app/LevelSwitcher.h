/// @file LevelSwitcher.h
/// @brief When a file is loaded this will find other levels that are in the same
///        directory as that file and add them to a menu so the user can quickly
///        switch between levels.

#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.common/Event.h>
#include "DirectoryListing.h"

namespace trview
{
    /// When a file is loaded this will find other levels that are in the same
    /// directory as that file and add them to a menu so the user can quickly
    /// switch between levels.
    class LevelSwitcher final : public MessageHandler
    {
    public:
        /// Create a new LevelSwitcher.
        /// @param The window to add the menu to.
        explicit LevelSwitcher(HWND window);

        /// Destructor for LevelSwitcher.
        virtual ~LevelSwitcher() = default;

        /// Handles a window message.
        /// @param window The window that received the message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Open the specified file. This will populate the switcher menu.
        /// @param filename The file that was opened.
        void open_file(const std::string& filename);

        /// Event raised when the user switches level. The opened level is passed as a parameter.
        Event<std::string> on_switch_level;
    private:

        HMENU             _directory_listing_menu;
        std::vector<File> _file_switcher_list;
    };
}

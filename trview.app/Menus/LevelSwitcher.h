/// @file LevelSwitcher.h
/// @brief When a file is loaded this will find other levels that are in the same
///        directory as that file and add them to a menu so the user can quickly
///        switch between levels.

#pragma once

#include "ILevelSwitcher.h"
#include <trview.common/MessageHandler.h>
#include "DirectoryListing.h"

namespace trview
{
    /// When a file is loaded this will find other levels that are in the same
    /// directory as that file and add them to a menu so the user can quickly
    /// switch between levels.
    class LevelSwitcher final : public ILevelSwitcher, public MessageHandler
    {
    public:
        /// Create a new LevelSwitcher.
        /// @param The window to add the menu to.
        explicit LevelSwitcher(const Window& window);

        /// Destructor for LevelSwitcher.
        virtual ~LevelSwitcher() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Open the specified file. This will populate the switcher menu.
        /// @param filename The file that was opened.
        virtual void open_file(const std::string& filename) override;
    private:
        HMENU             _directory_listing_menu;
        std::vector<File> _file_switcher_list;
    };
}

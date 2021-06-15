/// @file FileDropper.h
/// @brief When the user drags a file on to the window this will raise the appropriate event.

#pragma once

#include "IFileDropper.h"
#include <trview.common/MessageHandler.h>

namespace trview
{
    /// Raises events when the user drops a file on to the window.
    class FileDropper final : public IFileDropper, public MessageHandler
    {
    public:
        /// Create a new FileDropper.
        /// @param window The window that the user can drop files on to.
        explicit FileDropper(const Window& window);

        /// Destructor for FileDropper.
        virtual ~FileDropper() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
    };
}

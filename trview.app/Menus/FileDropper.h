/// @file FileDropper.h
/// @brief When the user drags a file on to the window this will raise the appropriate event.

#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.common/Event.h>
#include <string>

namespace trview
{
    /// Raises events when the user drops a file on to the window.
    class FileDropper final : public MessageHandler
    {
    public:
        /// Create a new FileDropper.
        /// @param window The window that the user can drop files on to.
        explicit FileDropper(HWND window);

        /// Destructor for FileDropper.
        virtual ~FileDropper() = default;

        /// Handles a window message.
        /// @param window The window that received the message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Event raised when the user drops a file on to the window. The file dropped is passed as a parameter.
        Event<std::string> on_file_dropped;
    };
}

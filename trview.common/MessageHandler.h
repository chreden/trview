/// @file MessageHandler.h
/// @brief Class that recieves Windows messages.

#pragma once

#include <cstdint>
#include <Windows.h>

namespace trview
{
    /// Base class for things that handle Windows messages.
    class MessageHandler
    {
    public:
        /// Create a new MessageHandler.
        /// @param window The window to listen to.
        explicit MessageHandler(HWND window);

        /// Destructor for MessageHandler.
        virtual ~MessageHandler();

        /// Handles a window message.
        /// @param window The window that received the message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) = 0;
    private:
        HWND     _window;
        uint32_t _id;
    };
}

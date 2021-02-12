/// @file MessageHandler.h
/// @brief Class that receives Windows messages.

#pragma once

#include <cstdint>
#include <Windows.h>
#include "Window.h"

namespace trview
{
    /// Base class for things that handle Windows messages.
    class MessageHandler
    {
    public:
        /// Create a new MessageHandler.
        /// @param window The window to listen to.
        explicit MessageHandler(const Window& window);

        /// Copy constructor for MessageHandler.
        /// @param other The handler to copy.
        MessageHandler(const MessageHandler& other);

        /// Move constructor for MessageHandler.
        /// @param other The handler to move from.
        MessageHandler(MessageHandler&& other);

        /// Copy assignment operator for MessageHandler.
        /// @param other The handler to copy.
        MessageHandler& operator=(const MessageHandler& other);

        /// Move assignment operator for MessageHandler.
        /// @param other The handler to move from.
        MessageHandler& operator=(MessageHandler&& other);

        /// Destructor for MessageHandler.
        virtual ~MessageHandler();

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) = 0;

        /// Gets the window that the handler is listening to.
        /// @returns The window handle.
        Window window() const;
    private:
        Window _window;
        uint32_t _subclass_id;
    };
}

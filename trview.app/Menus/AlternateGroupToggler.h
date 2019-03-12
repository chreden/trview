/// @file AlternateGroupToggler.h
/// @brief Watches for input and toggles alternate groups when the user presses number keys.

#pragma once

#include <cstdint>
#include <trview.common/MessageHandler.h>
#include <trview.common/Event.h>

namespace trview
{
    /// Watches for input and toggles alternate groups when the user presses number keys.
    class AlternateGroupToggler final : public MessageHandler
    {
    public:
        /// Create a new AlternateGroupToggler.
        /// @param window The window to monitor.
        explicit AlternateGroupToggler(HWND window);

        /// Handles a window message.
        /// @param window The window that received the message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Event raised when an alternate group is toggled by the user. The group
        /// is passed as a parameter.
        Event<uint32_t> on_alternate_group;
    };
}
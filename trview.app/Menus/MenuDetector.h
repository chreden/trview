#pragma once

#include <trview.common/Event.h>
#include <trview.common/MessageHandler.h>

namespace trview
{
    class MenuDetector final : public MessageHandler
    {
    public:
        /// Create a new MenuDetector.
        /// @param window The window that the detector is monitoring.
        explicit MenuDetector(const Window& window);

        /// Destructor for MenuDetector.
        virtual ~MenuDetector() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Event raised when the menu is toggled on or off.
        Event<bool> on_menu_toggled;
    private:

    };
}
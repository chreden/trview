/// @file WindowResizer.h
/// @brief When the window is resized this will raise an event.


#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.common/Event.h>
#include <trview.common/Size.h>

namespace trview
{
    /// Raises an event when the window is resized.
    class WindowResizer final : public MessageHandler
    {
    public:
        /// Create a WindowResizer.
        /// @param window The window to monitor.
        explicit WindowResizer(const Window& window);

        /// Destructor for WindowResizer.
        virtual ~WindowResizer() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Event that is raised when the window has resized.
        Event<> on_resize;

        static const int32_t WM_APP_PARENT_RESTORED;
    private:
        bool has_size_changed();

        bool _resizing{ false };
        Size _previous_size;
    };
}

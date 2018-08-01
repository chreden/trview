/// @file ItemsWindow.h
/// @brief Used to show and filter the items in the level.

#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.graphics/Device.h>
#include "WindowResizer.h"

namespace trview
{
    /// Used to show and filter the items in the level.
    class ItemsWindow final : public MessageHandler
    {
    public:
        /// Create an items window as a child of the specified window.
        /// @param parent The parent window.
        explicit ItemsWindow(const graphics::Device& device, HWND parent);

        /// Destructor for items window
        virtual ~ItemsWindow() = default;

        /// Handles a window message.
        /// @param window The window that received the message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Render the window and contents.
        /// @param vsync Whether to use vsync when rendering.
        void render(bool vsync);
    private:
        WindowResizer _window_resizer;
        std::unique_ptr<graphics::DeviceWindow> _device_window;
    };
}

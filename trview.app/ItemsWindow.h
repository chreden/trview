/// @file ItemsWindow.h
/// @brief Used to show and filter the items in the level.

#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.graphics/Device.h>
#include <trview.input/Mouse.h>
#include <trview.common/TokenStore.h>
#include "WindowResizer.h"

namespace trview
{
    namespace graphics
    {
        struct IShaderStorage;
        class FontFactory;
    }

    namespace ui
    {
        class Window;
        namespace render
        {
            class Renderer;
        }
    }

    /// Used to show and filter the items in the level.
    class ItemsWindow final : public MessageHandler
    {
    public:
        /// Create an items window as a child of the specified window.
        /// @param device The graphics device
        /// @param shader_storage The shader storage instance to use.
        /// @param font_factory The font factory to use.
        /// @param parent The parent window.
        explicit ItemsWindow(const graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, HWND parent);

        /// Destructor for items window
        virtual ~ItemsWindow() = default;

        /// Handles a window message.
        /// @param window The window that received the message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Render the window and contents.
        /// @param device The device to render with.
        /// @param vsync Whether to use vsync when rendering.
        void render(const graphics::Device& device, bool vsync);
    private:
        void generate_ui();

        WindowResizer _window_resizer;
        std::unique_ptr<graphics::DeviceWindow> _device_window;
        std::unique_ptr<ui::Window> _ui;
        std::unique_ptr<ui::render::Renderer> _ui_renderer;
        input::Mouse _mouse;
        TokenStore _token_store;
    };
}

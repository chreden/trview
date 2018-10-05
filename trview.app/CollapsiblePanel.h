#pragma once

#include <memory>
#include <string>
#include <trview.graphics/Device.h>
#include <trview.common/MessageHandler.h>
#include <trview.input/Mouse.h>
#include <trview.input/Keyboard.h>
#include <trview.common/TokenStore.h>
#include <trview.ui.render/Renderer.h>
#include <trview.graphics/DeviceWindow.h>

#include "WindowResizer.h"

namespace trview
{
    namespace graphics
    {
        class  Device;
        struct IShaderStorage;
        class  FontFactory;
    }

    namespace ui
    {
        class Control;
        class Window;
        class Button;
        namespace render
        {
            class Renderer;
        }
    }

    class CollapsiblePanel : public MessageHandler
    {
    public:
        /// Create a collapsible panel window as a child of the specified window.
        /// @param device The graphics device
        /// @param shader_storage The shader storage instance to use.
        /// @param font_factory The font factory to use.
        /// @param parent The parent window.
        explicit CollapsiblePanel(const graphics::Device& device,
            const graphics::IShaderStorage& shader_storage,
            const graphics::FontFactory& font_factory,
            HWND parent,
            const std::wstring& window_class,
            const std::wstring& title,
            const Size& size);

        virtual ~CollapsiblePanel() = default;

        /// Handles a window message.
        /// @param window The window that received the message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Render the window.
        /// @param device The device to render with.
        /// @param vsync Whether to use vsync or not.
        void render(const graphics::Device& device, bool vsync);

        /// Event raised when the items window is closed.
        Event<> on_window_closed;
    protected:
        virtual void update_layout();

        /// Add the expander button. This should be called by derived classes when they
        /// are ready to add the button. It will be added to the control specified as the 
        /// next child.
        /// @param parent The control to add the expand button to.
        void add_expander(ui::Control& parent);

        /// Set the left and right panels to use. This should be called before anything is done with the ui
        /// element as it will also generate the ui.
        void set_panels(std::unique_ptr<ui::Control> left_panel, std::unique_ptr<ui::Control> right_panel);

        TokenStore   _token_store;
        ui::Control* _left_panel;
        ui::Control* _right_panel;
    private:
        void toggle_expand();
        std::unique_ptr<ui::Control> create_divider();

        std::unique_ptr<graphics::DeviceWindow> _device_window;
        std::unique_ptr<ui::render::Renderer>   _ui_renderer;
        std::unique_ptr<ui::Window> _ui;
        WindowResizer   _window_resizer;
        input::Mouse    _mouse;
        input::Keyboard _keyboard;
        ui::Control* _divider;
        ui::Button* _expander;
        bool        _expanded{ true };
    };
}
/// @file A window with two panels where the right panel can be collapsed.

#pragma once

#include <memory>
#include <string>
#include <trview.common/MessageHandler.h>
#include <trview.common/TokenStore.h>
#include <trview.ui.render/IRenderer.h>
#include <trview.graphics/IDeviceWindow.h>
#include <trview.ui/IInput.h>
#include <trview.ui/Window.h>
#include <trview.app/Windows/WindowResizer.h>
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    namespace ui
    {
        class Control;
        class Button;
    }

    /// A window with two panels where the right panel can be collapsed.
    class CollapsiblePanel : public MessageHandler
    {
    public:
        /// Create a collapsible panel window as a child of the specified window.
        /// @param device_window_source The device window source function.
        /// @param ui_renderer The renderer to use.
        /// @param parent The parent window.
        /// @param window_class Window class name
        /// @param title Window title
        /// @param size Window size
        CollapsiblePanel(const graphics::IDeviceWindow::Source& device_window_source, std::unique_ptr<ui::render::IRenderer> ui_renderer, const Window& parent,
            const std::wstring& window_class, const std::wstring& title, const ui::IInput::Source& input_source, const Size& size);

        virtual ~CollapsiblePanel() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Render the window.
        /// @param device The device to render with.
        /// @param vsync Whether to use vsync or not.
        void render(bool vsync);

        /// Get the root control for the window.
        /// @returns The root control.
        ui::Control* root_control() const;

        /// Event raised when the window is closed.
        Event<> on_window_closed;

        Event<Size> on_size_changed;
    protected:
        virtual void update_layout();

        /// Add the expander button. This should be called by derived classes when they
        /// are ready to add the button. It will be added to the control specified as the 
        /// next child.
        /// @param parent The control to add the expand button to.
        void add_expander(ui::Control& parent);

        void set_expander(ui::Button* button);

        /// Set the left and right panels to use. This should be called before anything is done with the ui
        /// element as it will also generate the ui.
        void set_panels(std::unique_ptr<ui::Control> left_panel, std::unique_ptr<ui::Control> right_panel);

        /// Set whether the window can be made taller.
        /// @param Whether the window can be made taller.
        void set_allow_increase_height(bool value);

        void set_minimum_height(uint32_t height);

        TokenStore   _token_store;
        ui::Control* _left_panel;
        ui::Control* _right_panel;
        std::unique_ptr<ui::Window> _ui;
        std::unique_ptr<ui::IInput> _input;
        std::unique_ptr<graphics::IDeviceWindow> _device_window;
    private:
        void toggle_expand();
        std::unique_ptr<ui::Control> create_divider();
        void register_change_detection(ui::Control* control);

        Window _parent;
        std::unique_ptr<ui::render::IRenderer>   _ui_renderer;
        ui::Control* _panels;
        WindowResizer   _window_resizer;
        ui::Control* _divider;
        ui::Button* _expander;
        Size        _initial_size;
        bool        _allow_increase_height{ true };
        bool        _expanded{ true };
        bool        _ui_changed{ true };
    };
}
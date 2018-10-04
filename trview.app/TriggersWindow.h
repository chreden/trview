/// @file TriggersWindow.h
/// @brief Used to show and filter the triggers in the level.

#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.common/Event.h>
#include <trview.common/TokenStore.h>
#include <trview.graphics/Device.h>
#include <trview.input/Mouse.h>
#include <trview.input/Keyboard.h>
#include <trview.ui/Listbox.h>
#include "Item.h"
#include "Trigger.h"
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
        namespace render
        {
            class Renderer;
        }

        class Button;
        class Checkbox;
    }

    class TriggersWindow final : public MessageHandler
    {
    public:
        /// Create an items window as a child of the specified window.
        /// @param device The graphics device
        /// @param shader_storage The shader storage instance to use.
        /// @param font_factory The font factory to use.
        /// @param parent The parent window.
        explicit TriggersWindow(const graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, HWND parent);

        /// Destructor for triggers window
        virtual ~TriggersWindow() = default;

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

        /// Set the triggers to display in the window.
        /// @param triggers The triggers.
        void set_triggers(const std::vector<Trigger>& triggers);

        /// Clear the currently selected trigger from the details panel.
        void clear_selected_trigger();

        /// Event raised when a trigger is selected in the list.
        Event<Trigger> on_trigger_selected;

        /// Event raised when an item is selected in the list.
        Event<Item> on_item_selected;

        /// Event raised when the trigger window is closed.
        Event<> on_window_closed;

        /// Set the current room. This will be used when the track room setting is on.
        /// @param room The current room number.
        void set_current_room(uint32_t room);

        /// Set the selected item.
        /// @param item The selected item.
        void set_selected_trigger(const Trigger& item);
    private:
        void generate_ui();
        void update_layout();
        void populate_triggers(const std::vector<Trigger>& triggers);
        std::unique_ptr<ui::StackPanel> create_triggers_panel();
        std::unique_ptr<ui::Control> create_divider();
        void set_track_room(bool value);
        void set_sync_trigger(bool value);
        void toggle_expand();

        WindowResizer _window_resizer;
        std::unique_ptr<graphics::DeviceWindow> _device_window;
        std::unique_ptr<ui::Window> _ui;
        std::unique_ptr<ui::render::Renderer> _ui_renderer;
        input::Mouse _mouse;
        input::Keyboard _keyboard;
        TokenStore _token_store;

        ui::Window* _left_panel;
        ui::Window*  _controls;
        ui::Checkbox* _track_room_checkbox;
        ui::Button* _expander;
        ui::Listbox* _triggers_list;
        ui::Window* _divider;

        std::vector<Trigger> _all_triggers;

        /// Whether the trigger window is tracking the current room.
        bool _track_room{ false };
        /// The current room number selected for tracking.
        uint32_t _current_room{ 0u };
        /// Whether the room tracking filter has been applied.
        bool _filter_applied{ false };
        bool _sync_trigger{ true };
        std::optional<Trigger> _selected_trigger;
        bool _expanded{ true };
    };
}

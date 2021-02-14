/// @file ItemsWindowManager.h
/// @brief Controls and creates ItemsWindows.

#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <trview.common/MessageHandler.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IFontFactory.h>
#include <trview.common/TokenStore.h>
#include "ItemsWindow.h"
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    /// Controls and creates ItemsWindows.
    class ItemsWindowManager final : public MessageHandler
    {
    public:
        /// Create an ItemsWindowManager.
        /// @param device The device to use for items windows.
        /// @param shader_storage The shader storage for items windows.
        /// @param font_factory The font_factory for items windows.
        /// @param window The parent window of the items window.
        explicit ItemsWindowManager(graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::IFontFactory& font_factory, const Window& window, Shortcuts& shortcuts);

        /// Destructor for the ItemsWindowManager.
        virtual ~ItemsWindowManager() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Event raised when an item is selected in one of the item windows.
        Event<Item> on_item_selected;

        Event<Item, bool> on_item_visibility;

        /// Event raised when a trigger is selected in one of the item windows.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when the 'add to route' button is pressed in one of the item windows.
        Event<Item> on_add_to_route;

        /// Render all of the item windows.
        /// @param device The device to use to render.
        /// @param vsync Whether to use vsync.
        void render(graphics::Device& device, bool vsync);

        /// Set the items to use in the windows.
        /// @param items The items in the level.
        void set_items(const std::vector<Item>& items);

        /// Set whether an item is visible.
        /// @param item The item.
        /// @param visible Whether the item is visible.
        void set_item_visible(const Item& item, bool visible);

        /// Set the triggers to use in the windows.
        /// @param triggers The triggers in the level.
        void set_triggers(const std::vector<Trigger*>& triggers);

        /// Set the current room to filter item windows.
        /// @param room The current room.
        void set_room(uint32_t room);

        /// Set the currently selected item.
        /// @param item The selected item.
        void set_selected_item(const Item& item);

        /// Create a new items window.
        ItemsWindow* create_window();
    private:
        std::vector<std::unique_ptr<ItemsWindow>> _windows;
        std::vector<ItemsWindow*> _closing_windows;
        std::vector<Item> _items;
        std::vector<Trigger*> _triggers;
        graphics::Device& _device;
        const graphics::IShaderStorage& _shader_storage;
        const graphics::IFontFactory& _font_factory;
        uint32_t _current_room{ 0u };
        TokenStore _token_store;
        std::optional<Item> _selected_item;
    };
}


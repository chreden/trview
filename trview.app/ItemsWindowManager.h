/// @file ItemsWindowManager.h
/// @brief Controls and creates ItemsWindows.

#pragma once

#include <vector>
#include <memory>

#include <trview.common/MessageHandler.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.common/TokenStore.h>
#include "ItemsWindow.h"

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
        explicit ItemsWindowManager(graphics::Device& device, graphics::IShaderStorage& shader_storage, graphics::FontFactory& font_factory, HWND window);

        /// Destructor for the ItemsWindowManager.
        virtual ~ItemsWindowManager() = default;

        /// Handles a window message.
        /// @param window The window that received the message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(HWND window, UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Event raised when an item is selected in one of the item windows.
        Event<Item> on_item_selected;

        /// Render all of the item windows.
        /// @param device The device to use to render.
        /// @param vsync Whether to use vsync.
        void render(graphics::Device& device, bool vsync);

        /// Set the items to use in the windows.
        /// @param items The items in the level.
        void set_items(const std::vector<Item>& items);

        /// Set the current room to filter item windows.
        /// @param room The current room.
        void set_room(uint32_t room);

        /// Create a new items window.
        void create_window();
    private:
        std::vector<std::unique_ptr<ItemsWindow>> _windows;
        std::vector<ItemsWindow*> _closing_windows;
        std::vector<Item> _items;
        graphics::Device& _device;
        graphics::IShaderStorage& _shader_storage;
        graphics::FontFactory& _font_factory;
        uint32_t _current_room{ 0u };
        TokenStore _token_store;
    };
}


/// @file ItemsWindowManager.h
/// @brief Controls and creates ItemsWindows.

#pragma once

#include <vector>
#include <memory>
#include <optional>

#include "IItemsWindowManager.h"
#include <trview.common/MessageHandler.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/IFontFactory.h>
#include <trview.common/TokenStore.h>
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    /// Controls and creates ItemsWindows.
    class ItemsWindowManager final : public IItemsWindowManager, public MessageHandler
    {
    public:
        /// Create an ItemsWindowManager.
        /// @param device The device to use for items windows.
        /// @param shader_storage The shader storage for items windows.
        /// @param font_factory The font_factory for items windows.
        /// @param window The parent window of the items window.
        explicit ItemsWindowManager(const std::shared_ptr<graphics::IDevice>& device, const std::shared_ptr<graphics::IShaderStorage>& shader_storage, const std::shared_ptr<graphics::IFontFactory>& font_factory, const Window& window, const std::shared_ptr<IShortcuts>& shortcuts);

        /// Destructor for the ItemsWindowManager.
        virtual ~ItemsWindowManager() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Render all of the item windows.
        /// @param device The device to use to render.
        /// @param vsync Whether to use vsync.
        virtual void render(graphics::IDevice& device, bool vsync) override;

        /// Set the items to use in the windows.
        /// @param items The items in the level.
        virtual void set_items(const std::vector<Item>& items) override;

        /// Set whether an item is visible.
        /// @param item The item.
        /// @param visible Whether the item is visible.
        virtual void set_item_visible(const Item& item, bool visible) override;

        /// Set the triggers to use in the windows.
        /// @param triggers The triggers in the level.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) override;

        /// Set the current room to filter item windows.
        /// @param room The current room.
        virtual void set_room(uint32_t room) override;

        /// Set the currently selected item.
        /// @param item The selected item.
        virtual void set_selected_item(const Item& item) override;

        /// Create a new items window.
        virtual ItemsWindow* create_window() override;
    private:
        std::vector<std::unique_ptr<ItemsWindow>> _windows;
        std::vector<ItemsWindow*> _closing_windows;
        std::vector<Item> _items;
        std::vector<Trigger*> _triggers;
        std::shared_ptr<graphics::IDevice> _device;
        const std::shared_ptr<graphics::IShaderStorage> _shader_storage;
        const std::shared_ptr<graphics::IFontFactory> _font_factory;
        uint32_t _current_room{ 0u };
        TokenStore _token_store;
        std::optional<Item> _selected_item;
    };
}


/// @file ItemsWindowManager.h
/// @brief Controls and creates ItemsWindows.

#pragma once

#include <vector>
#include <memory>
#include <optional>

#include <trview.common/MessageHandler.h>
#include <trview.common/Windows/Shortcuts.h>
#include "IItemsWindowManager.h"
#include "WindowManager.h"

namespace trview
{
    /// Controls and creates ItemsWindows.
    class ItemsWindowManager final : public IItemsWindowManager, public WindowManager<IItemsWindow>, public MessageHandler
    {
    public:
        /// Create an TriggersWindowManager.
        /// @param window The parent window of the triggers window.
        /// @param shortcuts The shortcuts instance to use.
        /// @param items_window_source Function to call to create a triggers window.
        explicit ItemsWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IItemsWindow::Source& items_window_source);
        virtual ~ItemsWindowManager() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void render() override;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void set_item_visible(const Item& item, bool visible) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void set_room(uint32_t room) override;
        virtual void set_selected_item(const Item& item) override;
        virtual std::weak_ptr<IItemsWindow> create_window() override;
        virtual void update(float delta) override;
    private:
        std::vector<Item> _items;
        std::vector<std::weak_ptr<ITrigger>> _triggers;
        uint32_t _current_room{ 0u };
        std::optional<Item> _selected_item;
        IItemsWindow::Source _items_window_source;
    };
}

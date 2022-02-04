/// @file ItemsWindow.h
/// @brief Used to show and filter the items in the level.

#pragma once

#include <trview.common/Windows/IClipboard.h>

#include "IItemsWindow.h"
#include "../Elements/Item.h"

namespace trview
{
    /// Used to show and filter the items in the level.
    class ItemsWindow final : public IItemsWindow
    {
    public:
        struct Names
        {
            static inline const std::string add_to_route_button = "Add to Route";
            static inline const std::string sync_item = "Sync Item";
            static inline const std::string track_room = "Track Room";
        };

        explicit ItemsWindow(const Window& window, const std::shared_ptr<IClipboard>& clipboard);
        virtual ~ItemsWindow() = default;
        virtual void render(bool vsync) override;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void update_items(const std::vector<Item>& items) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void clear_selected_item() override;
        virtual void set_current_room(uint32_t room) override;
        virtual void set_selected_item(const Item& item) override;
        virtual std::optional<Item> selected_item() const override;
        virtual void update(float delta) override;
    private:
        void set_track_room(bool value);
        void set_sync_item(bool value);
        void render_items_list();
        void render_item_details();
        bool render_items_window();

        std::string _id;
        std::vector<Item> _all_items;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;
        /// Whether the item window is tracking the current room.
        bool _track_room{ false };
        /// The current room number selected for tracking.
        uint32_t _current_room{ 0u };
        bool _sync_item{ true };
        
        std::shared_ptr<IClipboard> _clipboard;
        std::unordered_map<std::string, std::string> _tips;
        std::optional<float> _tooltip_timer;
        std::weak_ptr<ITrigger> _selected_trigger;
        Window _window;

        // Item selection.
        std::optional<Item> _selected_item;
        std::optional<Item> _global_selected_item;
        bool _scroll_to_item{ false };
    };
}

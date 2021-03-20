/// @file ItemsWindow.h
/// @brief Used to show and filter the items in the level.

#pragma once

#include <trview.ui/Listbox.h>
#include <trview.app/Elements/Item.h>
#include "CollapsiblePanel.h"

namespace trview
{
    namespace ui
    {
        class Checkbox;
    }

    /// Used to show and filter the items in the level.
    class ItemsWindow final : public CollapsiblePanel
    {
    public:
        struct Names
        {
            static const std::string add_to_route_button;
            static const std::string items_listbox;
            static const std::string stats_listbox;
            static const std::string sync_item_checkbox;
            static const std::string track_room_checkbox;
            static const std::string triggers_listbox;
        };

        /// Create an items window as a child of the specified window.
        /// @param device The graphics device
        /// @param shader_storage The shader storage instance to use.
        /// @param font_factory The font factory to use.
        /// @param parent The parent window.
        explicit ItemsWindow(graphics::Device& device, const std::shared_ptr<graphics::IShaderStorage>& shader_storage, const graphics::IFontFactory& font_factory, const Window& parent);

        /// Destructor for items window
        virtual ~ItemsWindow() = default;

        /// Set the items to display in the window.
        /// @param items The items to show.
        void set_items(const std::vector<Item>& items);

        /// Update the items - this doesn't reset the filters.
        void update_items(const std::vector<Item>& items);

        /// Set the triggers to display in the window.
        /// @param triggers The triggers.
        void set_triggers(const std::vector<Trigger*>& triggers);

        /// Clear the currently selected item from the details panel.
        void clear_selected_item();

        /// Event raised when an item is selected in the list.
        Event<Item> on_item_selected;

        /// Event raised when an item visibility is changed.
        Event<Item, bool> on_item_visibility;

        /// Event raised when a trigger is selected in the list.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when the 'add to route' button is pressed.
        Event<Item> on_add_to_route;

        /// Set the current room. This will be used when the track room setting is on.
        /// @param room The current room number.
        void set_current_room(uint32_t room);

        /// Set the selected item.
        /// @param item The selected item.
        void set_selected_item(const Item& item);

        /// Get the selected item.
        /// @returns The selected item, if present.
        std::optional<Item> selected_item() const;
    protected:
        /// After the window has been resized, adjust the sizes of the child elements.
        virtual void update_layout() override;
    private:
        void populate_items(const std::vector<Item>& items);
        void load_item_details(const Item& item);
        void set_track_room(bool value);
        void set_sync_item(bool value);
        std::unique_ptr<ui::Control> create_left_panel();
        std::unique_ptr<ui::Control> create_right_panel();

        ui::StackPanel* _controls;
        ui::Listbox* _items_list;
        ui::Listbox* _stats_list;
        ui::Listbox* _trigger_list;
        ui::Checkbox* _track_room_checkbox;
        std::vector<Item> _all_items;
        std::vector<Trigger*> _all_triggers;
        /// Whether the item window is tracking the current room.
        bool _track_room{ false };
        /// The current room number selected for tracking.
        uint32_t _current_room{ 0u };
        /// Whether the room tracking filter has been applied.
        bool _filter_applied{ false };
        bool _sync_item{ true };
        std::optional<Item> _selected_item;
    };
}

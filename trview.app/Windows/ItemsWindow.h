/// @file ItemsWindow.h
/// @brief Used to show and filter the items in the level.

#pragma once

#include <trview.ui/Listbox.h>
#include <trview.app/Elements/Item.h>
#include "CollapsiblePanel.h"
#include "IItemsWindow.h"

namespace trview
{
    namespace ui
    {
        class Checkbox;
    }

    /// Used to show and filter the items in the level.
    class ItemsWindow final : public IItemsWindow, public CollapsiblePanel
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
        /// @param renderer_source The function to call to get a renderer.
        /// @param parent The parent window.
        explicit ItemsWindow(const std::shared_ptr<graphics::IDevice>& device, const ui::render::IRenderer::RendererSource& renderer_source, const Window& parent);

        /// Destructor for items window
        virtual ~ItemsWindow() = default;

        virtual void render(bool vsync) override;

        /// Set the items to display in the window.
        /// @param items The items to show.
        virtual void set_items(const std::vector<Item>& items) override;

        /// Update the items - this doesn't reset the filters.
        virtual void update_items(const std::vector<Item>& items) override;

        /// Set the triggers to display in the window.
        /// @param triggers The triggers.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) override;

        /// Clear the currently selected item from the details panel.
        virtual void clear_selected_item() override;

        /// Set the current room. This will be used when the track room setting is on.
        /// @param room The current room number.
        virtual void set_current_room(uint32_t room) override;

        /// Set the selected item.
        /// @param item The selected item.
        virtual void set_selected_item(const Item& item) override;

        /// Get the selected item.
        /// @returns The selected item, if present.
        virtual std::optional<Item> selected_item() const override;
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

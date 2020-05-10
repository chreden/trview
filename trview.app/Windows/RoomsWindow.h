#pragma once

#include <trview.common/Window.h>
#include <trview.ui/Listbox.h>
#include <trview.ui/Checkbox.h>
#include "CollapsiblePanel.h"
#include <trview.app/Elements/Item.h>

namespace trview
{
    class Room;
    class Trigger;

    class RoomsWindow final : public CollapsiblePanel
    {
    public:
        /// Create an rooms window as a child of the specified window.
        /// @param device The graphics device
        /// @param shader_storage The shader storage instance to use.
        /// @param font_factory The font factory to use.
        /// @param parent The parent window.
        explicit RoomsWindow(graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, const Window& parent);

        /// Destructor for rooms window
        virtual ~RoomsWindow() = default;

        void set_current_room(uint32_t room);

        void set_items(const std::vector<Item>& items);

        /// Set the rooms to display in the window.
        /// @param rooms The rooms to show.
        void set_rooms(const std::vector<Room*>& rooms);

        void set_triggers(const std::vector<Trigger*>& triggers);

        Event<uint32_t> on_room_selected;

        Event<Item> on_item_selected;

        Event<Trigger*> on_trigger_selected;
    private:
        void load_room_details(const Room& room);
        std::unique_ptr<ui::Control> create_left_panel();
        std::unique_ptr<ui::Control> create_right_panel();
        std::unique_ptr<ui::Listbox> create_rooms_list();
        void create_neighbours_list(ui::Control& parent);
        void create_items_list(ui::Control& parent);
        void create_triggers_list(ui::Control& parent);
        void set_track_room(bool value);

        std::vector<Room*> _all_rooms;
        std::vector<Item> _all_items;
        std::vector<Trigger*> _all_triggers;

        ui::Listbox* _rooms_list;
        ui::Listbox* _neighbours_list;
        ui::Listbox* _items_list;
        ui::Listbox* _triggers_list;
        ui::Listbox* _stats_box;
        ui::Checkbox* _track_room_checkbox;
        ui::Window* _controls;
        ui::Window* _minimap;
        bool _track_room{ true };
        bool _sync_item{ true };
        bool _sync_trigger{ true };
        uint32_t _current_room{ 0u };
    };
}

#pragma once

#include <trview.common/Window.h>
#include <trview.ui/Listbox.h>
#include <trview.ui/Checkbox.h>
#include <trview.ui/Image.h>
#include <trview.app/UI/Tooltip.h>
#include "CollapsiblePanel.h"
#include <trview.app/Elements/Item.h>
#include <trview.ui.render/MapRenderer.h>

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
        explicit RoomsWindow(graphics::IDevice& device, const std::shared_ptr<graphics::IShaderStorage>& shader_storage, const graphics::IFontFactory& font_factory, const Window& parent);

        /// Destructor for rooms window
        virtual ~RoomsWindow() = default;

        /// Set the current room that the viewer is focusing on.
        /// @param room The current room.
        void set_current_room(uint32_t room);

        /// Set the items in the level.
        /// @param items The items in the level.
        void set_items(const std::vector<Item>& items);

        /// Set the rooms to display in the window.
        /// @param rooms The rooms to show.
        void set_rooms(const std::vector<Room*>& rooms);

        /// Set the item currently selected in the viewer.
        /// @param item The item currently selected.
        void set_selected_item(const Item& item);

        /// Set the trigger currently selected in the viewer.
        /// @param trigger The trigger currently selected.
        void set_selected_trigger(const Trigger* const trigger);

        /// Set the triggers in the level.
        /// @param triggers The triggers in the level.
        void set_triggers(const std::vector<Trigger*>& triggers);

        /// Event raised when the user has selected a room in the room window.
        Event<uint32_t> on_room_selected;

        /// Event raised when the user has selected an item in the room window.
        Event<Item> on_item_selected;

        /// Event raised when the user has selected a trigger in the room window.
        Event<Trigger*> on_trigger_selected;
    private:
        void load_room_details(const Room& room);
        std::unique_ptr<ui::Control> create_left_panel();
        std::unique_ptr<ui::Control> create_right_panel();
        std::unique_ptr<ui::Listbox> create_rooms_list();
        void create_neighbours_list(ui::Control& parent);
        void create_items_list(ui::Control& parent);
        void create_triggers_list(ui::Control& parent);
        void set_sync_room(bool value);
        void set_track_item(bool value);
        void set_track_trigger(bool value);
        void render_minimap();

        std::vector<Room*> _all_rooms;
        std::vector<Item> _all_items;
        std::vector<Trigger*> _all_triggers;

        ui::Listbox* _rooms_list;
        ui::Listbox* _neighbours_list;
        ui::Listbox* _items_list;
        ui::Listbox* _triggers_list;
        ui::Listbox* _stats_box;
        ui::Checkbox* _sync_room_checkbox;
        ui::Checkbox* _track_trigger_checkbox;
        ui::Checkbox* _track_item_checkbox;
        ui::Window* _controls;
        ui::Image* _minimap;
        bool _sync_room{ true };
        bool _track_item{ false };
        bool _track_trigger{ false };
        uint32_t _current_room{ 0u };
        std::optional<Item> _selected_item;
        std::optional<const Trigger*> _selected_trigger;

        std::unique_ptr<ui::render::MapRenderer> _map_renderer;
        std::unique_ptr<Tooltip> _map_tooltip;
    };
}

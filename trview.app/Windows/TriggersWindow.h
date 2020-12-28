/// @file TriggersWindow.h
/// @brief Used to show and filter the triggers in the level.

#pragma once

#include <trview.ui/Listbox.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Trigger.h>
#include "CollapsiblePanel.h"
#include <trview.graphics/IFontFactory.h>

namespace trview
{
    namespace ui
    {
        class Checkbox;
        class Dropdown;
    }

    class TriggersWindow final : public CollapsiblePanel
    {
    public:
        struct Names
        {
            static const std::string add_to_route_button;
            static const std::string filter_dropdown;
            static const std::string sync_trigger_checkbox;
            static const std::string triggers_listbox;
            static const std::string trigger_commands_listbox;
        };

        /// Create an items window as a child of the specified window.
        /// @param device The graphics device
        /// @param shader_storage The shader storage instance to use.
        /// @param font_factory The font factory to use.
        /// @param parent The parent window.
        explicit TriggersWindow(graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::IFontFactory& font_factory, const Window& parent);

        /// Destructor for triggers window
        virtual ~TriggersWindow() = default;

        /// Set the triggers to display in the window.
        /// @param triggers The triggers.
        /// @param reset_filters Whether to reset the trigger filters.
        void set_triggers(const std::vector<Trigger*>& triggers);

        /// Update the trigers - this doesn't reset the filters.
        void update_triggers(const std::vector<Trigger*>& triggers);

        /// Clear the currently selected trigger from the details panel.
        void clear_selected_trigger();

        /// Event raised when a trigger is selected in the list.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when the visibility of a trigger is changed.
        Event<Trigger*, bool> on_trigger_visibility;

        /// Event raised when an item is selected in the list.
        Event<Item> on_item_selected;

        /// Event raised when the 'add to route' button is pressed.
        Event<const Trigger*> on_add_to_route;

        /// Set the current room. This will be used when the track room setting is on.
        /// @param room The current room number.
        void set_current_room(uint32_t room);

        /// Set the selected item.
        /// @param item The selected item.
        void set_selected_trigger(const Trigger* const item);

        void set_items(const std::vector<Item>& items);
    protected:
        virtual void update_layout() override;
    private:
        void populate_triggers(const std::vector<Trigger*>& triggers);
        std::unique_ptr<ui::Control> create_left_panel();
        std::unique_ptr<ui::Control> create_right_panel();
        void set_track_room(bool value);
        void set_sync_trigger(bool value);
        void load_trigger_details(const Trigger& trigger);
        void apply_filters();

        ui::StackPanel* _controls;
        ui::Checkbox* _track_room_checkbox;
        ui::Listbox*  _triggers_list;
        ui::Listbox*  _stats_list;
        ui::Listbox*  _command_list;
        ui::Dropdown* _command_filter;

        std::vector<Item> _all_items;
        std::vector<Trigger*> _all_triggers;

        /// Whether the trigger window is tracking the current room.
        bool _track_room{ false };
        /// The current room number selected for tracking.
        uint32_t _current_room{ 0u };
        /// Whether the room tracking filter has been applied.
        bool _filter_applied{ false };
        bool _sync_trigger{ true };
        std::optional<const Trigger*> _selected_trigger;
        std::vector<TriggerCommandType> _selected_commands;
    };
}

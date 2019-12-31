
#pragma once

#include <trview.ui/Listbox.h>
#include <trview.ui/TextArea.h>
#include <trview.ui/Dropdown.h>
#include "CollapsiblePanel.h"
#include <trview.common/Event.h>
#include <trview.app/Routing/Waypoint.h>
#include <trview.app/Elements/Item.h>

namespace trview
{
    class Route;

    class RouteWindow final : public CollapsiblePanel
    {
    public:
        /// Create an items window as a child of the specified window.
        /// @param device The graphics device
        /// @param shader_storage The shader storage instance to use.
        /// @param font_factory The font factory to use.
        /// @param parent The parent window.
        explicit RouteWindow(graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, const trview::Window& parent);

        /// Destructor for triggers window
        virtual ~RouteWindow() = default;

        /// Load the waypoints from the route.
        /// @param route The route to load from.
        void set_route(Route* route);

        /// Event raised when a waypoint is selected.
        Event<uint32_t> on_waypoint_selected;

        /// Event raised when an item is selected.
        Event<Item> on_item_selected;

        /// Event raised when a trigger is selected.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when the route colour has been changed.
        Event<Colour> on_colour_changed;

        /// Event raised when a route file is opened.
        Event<std::string> on_route_import;

        /// Event raised when a route is exported.
        Event<std::string> on_route_export;

        /// Event raised when a waypoint is deleted.
        Event<uint32_t> on_waypoint_deleted;

        /// Select the specified waypoint.
        /// @param index The index of the waypoint to select.
        void select_waypoint(uint32_t index);

        /// Set the items to that are in the level.
        /// @param items The items to show.
        void set_items(const std::vector<Item>& items);

        /// Set the triggers in the level.
        /// @param triggers The triggers.
        void set_triggers(const std::vector<Trigger*>& triggers);
    private:
        void load_waypoint_details(uint32_t index);
        std::unique_ptr<ui::Control> create_left_panel();
        std::unique_ptr<ui::Control> create_right_panel();
        ui::Listbox::Item create_listbox_item(uint32_t index, const Waypoint& waypoint);

        ui::Dropdown* _colour;
        ui::Listbox* _waypoints;
        ui::Listbox* _stats;
        ui::TextArea* _notes_area;
        Route* _route{ nullptr };
        std::vector<Item> _all_items;
        std::vector<Trigger*> _all_triggers;
        Waypoint::Type _selected_type{ Waypoint::Type::Position };
        uint32_t       _selected_index{ 0u };
    };
}

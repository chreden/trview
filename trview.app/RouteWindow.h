#pragma once

#include <trview.ui/Listbox.h>
#include "CollapsiblePanel.h"
#include <trview.common/Event.h>
#include "Waypoint.h"
#include "Item.h"

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
        explicit RouteWindow(const graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, HWND parent);

        /// Destructor for triggers window
        virtual ~RouteWindow() = default;

        /// Load the waypoints from the route.
        /// @param route The route to load from.
        void load_waypoints(const Route& route);

        /// Event raised when a waypoint is selected.
        Event<uint32_t> on_waypoint_selected;

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
        ui::Listbox* _waypoints;
        ui::Listbox* _stats;
        std::vector<Waypoint> _all_waypoints;
        std::vector<Item> _all_items;
        std::vector<Trigger*> _all_triggers;
    };
}

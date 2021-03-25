
#pragma once

#include <trview.ui/Listbox.h>
#include <trview.ui/TextArea.h>
#include <trview.ui/Dropdown.h>
#include "CollapsiblePanel.h"
#include <trview.common/Event.h>
#include <trview.app/Routing/Waypoint.h>
#include <trview.app/Elements/Item.h>
#include <trview.app/Elements/Room.h>
#include <trview.graphics/FontFactory.h>
#include "IRouteWindow.h"

namespace trview
{
    struct IRoute;

    class RouteWindow final : public IRouteWindow, public CollapsiblePanel
    {
    public:
        /// Create a route window as a child of the specified window.
        /// @param device The graphics device
        /// @param renderer_source The function to call to get a renderer.
        /// @param parent The parent window.
        explicit RouteWindow(const std::shared_ptr<graphics::IDevice>& device, const ui::render::IRenderer::RendererSource& renderer_source, const trview::Window& parent);

        /// Destructor for triggers window
        virtual ~RouteWindow() = default;

        virtual void render(const graphics::IDevice& device, bool vsync) override;

        /// Load the waypoints from the route.
        /// @param route The route to load from.
        virtual void set_route(IRoute* route) override;

        /// Select the specified waypoint.
        /// @param index The index of the waypoint to select.
        virtual void select_waypoint(uint32_t index) override;

        /// Set the items to that are in the level.
        /// @param items The items to show.
        virtual void set_items(const std::vector<Item>& items) override;

        virtual void set_rooms(const std::vector<Room*>& rooms) override;

        /// Set the triggers in the level.
        /// @param triggers The triggers.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) override;

        virtual void focus() override;
    private:
        void load_waypoint_details(uint32_t index);
        std::unique_ptr<ui::Control> create_left_panel();
        std::unique_ptr<ui::Control> create_right_panel();
        ui::Listbox::Item create_listbox_item(uint32_t index, const Waypoint& waypoint);

        ui::Dropdown* _colour;
        ui::Listbox* _waypoints;
        ui::Listbox* _stats;
        ui::TextArea* _notes_area;
        ui::Button* _select_save;
        ui::Button* _delete_waypoint;
        ui::Button* _clear_save;
        IRoute* _route{ nullptr };
        std::vector<Item> _all_items;
        std::vector<Room*> _all_rooms;
        std::vector<Trigger*> _all_triggers;
        Waypoint::Type _selected_type{ Waypoint::Type::Position };
        uint32_t       _selected_index{ 0u };
    };
}

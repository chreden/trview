#pragma once

#include <trview.common/MessageHandler.h>
#include <trview.common/TokenStore.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include "RouteWindow.h"
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    class Shortcuts;

    /// Controls and creates the route window.
    class RouteWindowManager final : public MessageHandler
    {
    public:
        explicit RouteWindowManager(graphics::Device& device, const graphics::IShaderStorage& shader_storage, const graphics::FontFactory& font_factory, const Window& window, Shortcuts& shortcuts);

        virtual ~RouteWindowManager() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Render all of the route windows.
        /// @param device The device to use to render.
        /// @param vsync Whether to use vsync.
        void render(graphics::Device& device, bool vsync);

        /// Load the waypoints from the route.
        /// @param route The route to load from.
        void set_route(Route* route);

        /// Create a new route window.
        void create_window();

        /// Set the items to that are in the level.
        /// @param items The items to show.
        void set_items(const std::vector<Item>& items);

        /// Set the triggers in the level.
        /// @param triggers The triggers.
        void set_triggers(const std::vector<Trigger*>& triggers);

        void select_waypoint(uint32_t index);

        /// Event raised when the route colour has changed.
        Event<Colour> on_colour_changed;

        /// Event raised when a route is imported.
        Event<std::string> on_route_import;

        /// Event raised when a route is exported.
        Event<std::string> on_route_export;

        /// Event raised when a waypoint is selected.
        Event<uint32_t> on_waypoint_selected;

        /// Event raised when an item is selected.
        Event<Item> on_item_selected;

        /// Event raised when a trigger is selected.
        Event<Trigger*> on_trigger_selected;

        /// Event raised when a waypoint is deleted.
        Event<uint32_t> on_waypoint_deleted;
    private:
        graphics::Device& _device;
        const graphics::IShaderStorage& _shader_storage;
        const graphics::FontFactory& _font_factory;
        TokenStore _token_store;
        std::unique_ptr<RouteWindow> _route_window;
        bool _closing{ false };
        Route* _route{ nullptr };
        std::vector<Item> _all_items;
        std::vector<Trigger*> _all_triggers;
        uint32_t _selected_waypoint{ 0u };
    };
}

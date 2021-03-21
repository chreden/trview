#pragma once

#include <trview.app/Windows/IRouteWindowManager.h>
#include <trview.common/MessageHandler.h>
#include <trview.common/TokenStore.h>
#include <trview.graphics/Device.h>
#include <trview.graphics/IShaderStorage.h>
#include <trview.graphics/FontFactory.h>
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    /// Controls and creates the route window.
    class RouteWindowManager final : public IRouteWindowManager, public MessageHandler
    {
    public:
        explicit RouteWindowManager(graphics::IDevice& device, const std::shared_ptr<graphics::IShaderStorage>& shader_storage, const graphics::IFontFactory& font_factory, const Window& window, IShortcuts& shortcuts);

        virtual ~RouteWindowManager() = default;

        /// Handles a window message.
        /// @param message The message that was received.
        /// @param wParam The WPARAM for the message.
        /// @param lParam The LPARAM for the message.
        virtual void process_message(UINT message, WPARAM wParam, LPARAM lParam) override;

        /// Render all of the route windows.
        /// @param device The device to use to render.
        /// @param vsync Whether to use vsync.
        virtual void render(graphics::IDevice& device, bool vsync) override;

        /// Load the waypoints from the route.
        /// @param route The route to load from.
        virtual void set_route(IRoute* route) override;

        /// Create a new route window.
        virtual void create_window() override;

        /// Set the items to that are in the level.
        /// @param items The items to show.
        virtual void set_items(const std::vector<Item>& items) override;

        virtual void set_rooms(const std::vector<Room*>& rooms) override;

        /// Set the triggers in the level.
        /// @param triggers The triggers.
        virtual void set_triggers(const std::vector<Trigger*>& triggers) override;

        virtual void select_waypoint(uint32_t index) override;
    private:
        graphics::IDevice& _device;
        const std::shared_ptr<graphics::IShaderStorage> _shader_storage;
        const graphics::IFontFactory& _font_factory;
        TokenStore _token_store;
        std::unique_ptr<RouteWindow> _route_window;
        bool _closing{ false };
        IRoute* _route{ nullptr };
        std::vector<Item> _all_items;
        std::vector<Room*> _all_rooms;
        std::vector<Trigger*> _all_triggers;
        uint32_t _selected_waypoint{ 0u };
    };
}

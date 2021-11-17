#pragma once

#include <trview.app/Windows/IRouteWindowManager.h>
#include <trview.common/MessageHandler.h>
#include <trview.common/TokenStore.h>
#include <trview.common/Windows/Shortcuts.h>

namespace trview
{
    /// Controls and creates the route window.
    class RouteWindowManager final : public IRouteWindowManager, public MessageHandler
    {
    public:
        explicit RouteWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IRouteWindow::Source& route_window_source);
        virtual ~RouteWindowManager() = default;
        virtual std::optional<int> process_message(UINT message, WPARAM wParam, LPARAM lParam) override;
        virtual void render(bool vsync) override;
        virtual void set_route(IRoute* route) override;
        virtual void create_window() override;
        virtual void set_items(const std::vector<Item>& items) override;
        virtual void set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms) override;
        virtual void set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers) override;
        virtual void select_waypoint(uint32_t index) override;
        virtual void update(float delta) override;
        virtual void set_randomizer_enabled(bool value) override;
        virtual void set_randomizer_settings(const RandomizerSettings& settings) override;
    private:
        TokenStore _token_store;
        std::shared_ptr<IRouteWindow> _route_window;
        bool _closing{ false };
        IRoute* _route{ nullptr };
        std::vector<Item> _all_items;
        std::vector<std::weak_ptr<IRoom>> _all_rooms;
        std::vector<std::weak_ptr<ITrigger>> _all_triggers;
        uint32_t _selected_waypoint{ 0u };
        IRouteWindow::Source _route_window_source;
        bool _randomizer_enabled{ false };
        RandomizerSettings _randomizer_settings;
    };
}

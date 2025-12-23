#include "RouteWindowManager.h"
#include "../Resources/resource.h"

namespace trview
{
    IRouteWindowManager::~IRouteWindowManager()
    {
    }

    RouteWindowManager::RouteWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IRouteWindow::Source& route_window_source)
        : MessageHandler(window), _route_window_source(route_window_source)
    {
        _token_store += shortcuts->add_shortcut(true, 'R') += [&]() { create_window(); };
    }

    std::optional<int> RouteWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_WINDOWS_ROUTE)
        {
            create_window();
        }
        return {};
    }

    void RouteWindowManager::create_window()
    {
        // If the window already exists, just focus on the window.
        if (_route_window)
        {
            _route_window->focus();
            return;
        }

        // Otherwise create the window.
        _route_window = _route_window_source();
        _route_window->on_route_open += on_route_open;
        _route_window->on_route_reload += on_route_reload;
        _route_window->on_route_save += on_route_save;
        _route_window->on_route_save_as += on_route_save_as;
        _route_window->on_waypoint_selected += on_waypoint_selected;
        _route_window->on_level_switch += on_level_switch;
        _route_window->on_new_route += on_new_route;
        _route_window->on_new_randomizer_route += on_new_randomizer_route;
        _token_store += _route_window->on_window_closed += [&]() { _closing = true; };

        _route_window->set_route(_route);
        _route_window->select_waypoint(_selected_waypoint);
        on_window_created();
    }

    void RouteWindowManager::render()
    {
        if (_closing)
        {
            _route_window.reset();
            _closing = false;
        }

        if (_route_window)
        {
            _route_window->render();
        }
    }

    void RouteWindowManager::set_route(const std::weak_ptr<IRoute>& route)
    {
        _route = route;
        if (_route_window)
        {
            _route_window->set_route(route);
        }
    }

    void RouteWindowManager::select_waypoint(const std::weak_ptr<IWaypoint>& waypoint)
    {
        _selected_waypoint = waypoint;
        if (_route_window)
        {
            _route_window->select_waypoint(waypoint);
        }
    }

    void RouteWindowManager::update(float delta)
    {
        if (_route_window)
        {
            _route_window->update(delta);
        }
    }

    bool RouteWindowManager::is_window_open() const
    {
        return _route_window.get();
    }
}
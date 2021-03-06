#include "RouteWindowManager.h"
#include <trview.app/Windows/WindowIDs.h>

namespace trview
{
    RouteWindowManager::RouteWindowManager(const Window& window, const std::shared_ptr<IShortcuts>& shortcuts, const IRouteWindow::Source& route_window_source)
        : MessageHandler(window), _route_window_source(route_window_source)
    {
        _token_store += shortcuts->add_shortcut(true, 'R') += [&]() { create_window(); };
    }

    std::optional<int> RouteWindowManager::process_message(UINT message, WPARAM wParam, LPARAM)
    {
        if (message == WM_COMMAND && LOWORD(wParam) == ID_APP_WINDOWS_ROUTE)
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
        _route_window = _route_window_source(window());
        _route_window->on_colour_changed += on_colour_changed;
        _route_window->on_route_import += on_route_import;
        _route_window->on_route_export += on_route_export;
        _route_window->on_item_selected += on_item_selected;
        _route_window->on_trigger_selected += on_trigger_selected;
        _route_window->on_waypoint_selected += on_waypoint_selected;
        _route_window->on_waypoint_deleted += on_waypoint_deleted;
        _token_store += _route_window->on_window_closed += [&]() { _closing = true; };

        _route_window->set_items(_all_items);
        _route_window->set_rooms(_all_rooms);
        _route_window->set_triggers(_all_triggers);
        if (_route)
        {
            _route_window->set_route(_route);
        }
        _route_window->select_waypoint(_selected_waypoint);
    }

    void RouteWindowManager::render(bool vsync)
    {
        if (_closing)
        {
            _route_window.reset();
            _closing = false;
        }

        if (_route_window)
        {
            _route_window->render(vsync);
        }
    }

    void RouteWindowManager::set_route(IRoute* route)
    {
        _route = route;
        if (_route_window)
        {
            _route_window->set_route(route);
        }
    }

    void RouteWindowManager::set_items(const std::vector<Item>& items)
    {
        _all_items = items;

        if (_route_window)
        {
            _route_window->set_items(items);
        }
    }

    void RouteWindowManager::set_rooms(const std::vector<std::weak_ptr<IRoom>>& rooms)
    {
        _all_rooms = rooms;
        if (_route_window)
        {
            _route_window->set_rooms(rooms);
        }
    }

    void RouteWindowManager::set_triggers(const std::vector<std::weak_ptr<ITrigger>>& triggers)
    {
        _all_triggers = triggers;
        if (_route_window)
        {
            _route_window->set_triggers(triggers);
        }
    }

    void RouteWindowManager::select_waypoint(uint32_t index)
    {
        _selected_waypoint = index;

        if (_route_window)
        {
            _route_window->select_waypoint(index);
        }
    }
}
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
        _route_window->on_item_selected += on_item_selected;
        _route_window->on_trigger_selected += on_trigger_selected;
        _route_window->on_waypoint_selected += on_waypoint_selected;
        _route_window->on_scene_changed += on_scene_changed;
        _route_window->on_level_switch += on_level_switch;
        _route_window->on_new_route += on_new_route;
        _route_window->on_new_randomizer_route += on_new_randomizer_route;
        _token_store += _route_window->on_window_closed += [&]() { _closing = true; };

        _route_window->set_randomizer_settings(_randomizer_settings);
        _route_window->set_randomizer_enabled(_randomizer_enabled);
        _route_window->set_items(_all_items);
        _route_window->set_rooms(_all_rooms);
        _route_window->set_triggers(_all_triggers);
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

    void RouteWindowManager::set_items(const std::vector<std::weak_ptr<IItem>>& items)
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

    void RouteWindowManager::set_randomizer_enabled(bool value)
    {
        _randomizer_enabled = value;
        if (_route_window)
        {
            _route_window->set_randomizer_enabled(value);
        }
    }

    void RouteWindowManager::set_randomizer_settings(const RandomizerSettings& settings)
    {
        _randomizer_settings = settings;
        if (_route_window)
        {
            _route_window->set_randomizer_settings(settings);
        }
    }

    bool RouteWindowManager::is_window_open() const
    {
        return _route_window.get();
    }
}
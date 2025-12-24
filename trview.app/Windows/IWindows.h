#pragma once

#include <memory>
#include <trview.common/Event.h>

namespace trview
{
    struct IFlybyNode;
    struct ILevel;
    struct ILight;
    struct IRoom;
    struct IRoute;
    struct ISector;
    struct IStaticMesh;
    struct ITrigger;
    struct IWaypoint;

    struct IWindows
    {
        virtual ~IWindows() = 0;
        virtual bool is_route_window_open() const = 0;
        virtual void update(float elapsed) = 0;
        virtual void render() = 0;
        virtual void select(const std::weak_ptr<IWaypoint>& waypoint) = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual void set_route(const std::weak_ptr<IRoute>& route) = 0;
        virtual void setup(const UserSettings& settings) = 0;

        Event<std::string> on_level_open;
        Event<std::string> on_level_switch;
        Event<> on_new_route;
        Event<> on_new_randomizer_route;
        Event<> on_route_open;
        Event<> on_route_reload;
        Event<> on_route_save;
        Event<> on_route_save_as;
        Event<std::weak_ptr<IWaypoint>> on_waypoint_selected;
        Event<> on_route_window_created;
    };
}

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
    struct IWindow;
    struct UserSettings;

    struct IWindows
    {
        virtual ~IWindows() = 0;
        virtual std::weak_ptr<IWindow> create(const std::string& type) = 0;
        virtual void update(float elapsed) = 0;
        virtual void render() = 0;
        virtual void setup(const UserSettings& settings) = 0;
        virtual std::vector<std::weak_ptr<IWindow>> windows(const std::string& type) const = 0;
        Event<> on_route_window_created;
    };
}

#pragma once

#include <memory>
#include <trview.common/Event.h>

namespace trview
{
    struct ILevel;
    struct IRoom;
    struct IStaticMesh;
    struct UserSettings;

    struct IWindows
    {
        virtual ~IWindows() = 0;
        virtual void update(float elapsed) = 0;
        virtual void render() = 0;
        virtual void select(const std::weak_ptr<IStaticMesh>& static_mesh) = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void setup(const UserSettings& settings) = 0;
        Event<std::weak_ptr<IStaticMesh>> on_static_selected;
    };
}

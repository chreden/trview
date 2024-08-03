#pragma once

#include <memory>
#include <trview.common/Event.h>

namespace trview
{
    struct ICameraSink;
    struct IItem;
    struct ILevel;
    struct IRoom;
    struct IStaticMesh;
    struct ITrigger;
    struct UserSettings;

    struct IWindows
    {
        virtual ~IWindows() = 0;
        virtual void update(float elapsed) = 0;
        virtual void render() = 0;
        virtual void select(const std::weak_ptr<ICameraSink>& camera_sink) = 0;
        virtual void select(const std::weak_ptr<IItem>& item) = 0;
        virtual void select(const std::weak_ptr<ILight>& light) = 0;
        virtual void select(const std::weak_ptr<IStaticMesh>& static_mesh) = 0;
        virtual void select(const std::weak_ptr<ITrigger>& trigger) = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void setup(const UserSettings& settings) = 0;

        Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
        Event<std::weak_ptr<IItem>> on_item_selected;
        Event<std::weak_ptr<ILight>> on_light_selected;
        Event<std::weak_ptr<IStaticMesh>> on_static_selected;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;
        Event<> on_scene_changed;
    };
}

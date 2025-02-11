#pragma once

#include "ITriggersWindow.h"

namespace trview
{
    struct ITriggersWindowManager
    {
        virtual ~ITriggersWindowManager() = 0;

        Event<std::weak_ptr<ITrigger>> on_add_to_route;
        Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
        Event<std::weak_ptr<IItem>> on_item_selected;
        Event<> on_scene_changed;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;

        virtual void add_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual void render() = 0;
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) = 0;
        virtual std::weak_ptr<ITriggersWindow> create_window() = 0;
        virtual void update(float delta) = 0;
    };
}

#pragma once

#include <vector>
#include <trview.common/Event.h>
#include "../Elements/IItem.h"
#include "../Elements/ITrigger.h"
#include "../Elements/CameraSink/ICameraSink.h"

namespace trview
{
    struct ITriggersWindow
    {
        using Source = std::function<std::shared_ptr<ITriggersWindow>()>;

        Event<std::weak_ptr<ITrigger>> on_add_to_route;
        Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
        Event<std::weak_ptr<IItem>> on_item_selected;
        Event<> on_scene_changed;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;
        Event<> on_window_closed;

        virtual ~ITriggersWindow() = 0;
        virtual void add_level(const std::weak_ptr<ILevel>& level) = 0;
        virtual void render() = 0;
        virtual void set_current_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_selected_trigger(const std::weak_ptr<ITrigger>& trigger) = 0;
        virtual void update(float delta) = 0;
    };
}


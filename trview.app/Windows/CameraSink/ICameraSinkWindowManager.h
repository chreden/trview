#pragma once

#include "ICameraSinkWindow.h"
#include "../../Elements/ITrigger.h"

namespace trview
{
    struct ICameraSinkWindowManager
    {
        virtual ~ICameraSinkWindowManager() = 0;
        virtual std::weak_ptr<ICameraSinkWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks) = 0;
        virtual void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) = 0;
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;

        Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;
        Event<> on_scene_changed;
    };
}

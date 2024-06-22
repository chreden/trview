#pragma once

#include "../../Elements/CameraSink/ICameraSink.h"
#include "../../Elements/ITrigger.h"

namespace trview
{
    struct ICameraSinkWindow
    {
        using Source = std::function<std::shared_ptr<ICameraSinkWindow>()>;

        virtual ~ICameraSinkWindow() = 0;
        virtual void render() = 0;
        virtual void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks) = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_selected_camera_sink(const std::weak_ptr<ICameraSink>& camera_sink) = 0;
        virtual void set_current_room(const std::weak_ptr<IRoom>& room) = 0;
        /// <summary>
        /// Event raised when the window is closed.
        /// </summary>
        Event<> on_window_closed;

        Event<std::weak_ptr<ICameraSink>> on_camera_sink_selected;
        Event<std::weak_ptr<ITrigger>> on_trigger_selected;
        Event<> on_camera_sink_type_changed;
    };
}

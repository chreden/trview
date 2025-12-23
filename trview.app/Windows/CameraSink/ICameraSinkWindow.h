#pragma once

#include "../../Elements/CameraSink/ICameraSink.h"
#include "../../Elements/ITrigger.h"
#include "../../Elements/Flyby/IFlyby.h"

namespace trview
{
    struct ICameraSinkWindow
    {
        using Source = std::function<std::shared_ptr<ICameraSinkWindow>()>;

        virtual ~ICameraSinkWindow() = 0;
        virtual void render() = 0;
        virtual void set_number(int32_t number) = 0;
        virtual void set_current_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void update(float delta) = 0;
        /// <summary>
        /// Event raised when the window is closed.
        /// </summary>
        Event<> on_window_closed;
    };
}

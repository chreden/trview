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
        virtual void set_room(const std::weak_ptr<IRoom>& room) = 0;
        virtual void update(float delta) = 0;
    };
}

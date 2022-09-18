#pragma once

#include "ICameraSinkWindow.h"

namespace trview
{
    struct ICameraSinkWindowManager
    {
        virtual ~ICameraSinkWindowManager() = 0;
        virtual std::weak_ptr<ICameraSinkWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void set_camera_sinks(const std::vector<std::weak_ptr<ICameraSink>>& camera_sinks) = 0;
    };
}

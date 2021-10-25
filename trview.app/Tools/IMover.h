#pragma once

#include <trview.app/Camera/ICamera.h>

namespace trview
{
    struct IMover
    {
        virtual ~IMover() = 0;
        virtual void render(const ICamera& camera) = 0;
    };
}

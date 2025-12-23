#pragma once

#include <vector>
#include <memory>
#include <trview.app/Elements/ILight.h>
#include "ILightsWindow.h"

namespace trview
{
    struct ILightsWindowManager
    {
        virtual ~ILightsWindowManager() = 0;
        virtual std::weak_ptr<ILightsWindow> create_window() = 0;
        virtual void render() = 0;
        virtual void update(float delta) = 0;
    };
}

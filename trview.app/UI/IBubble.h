#pragma once

#include <trview.common/Point.h>
#include <trview.ui/Control.h>

namespace trview
{
    struct IBubble
    {
        using Source = std::function<std::unique_ptr<IBubble>(ui::Control&)>;

        virtual ~IBubble() = 0;
        virtual void show(const Point& position) = 0;
    };
}

#pragma once

import trview.common.point;
#include <trview.ui/Control.h>

namespace trview
{
    /// <summary>
    /// Shows a short lived notification bubble.
    /// </summary>
    struct IBubble
    {
        using Source = std::function<std::unique_ptr<IBubble>(ui::Control&)>;
        virtual ~IBubble() = 0;
        /// <summary>
        /// Show the notification bubble at the specified position.
        /// </summary>
        /// <param name="position">The point to show the notification bubble.</param>
        virtual void show(const Point& position) = 0;
    };
}

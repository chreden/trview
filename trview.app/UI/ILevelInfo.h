#pragma once

#include <memory>

#include <trview.common/Event.h>

namespace trview
{
    struct ILevel;
    struct ILevelInfo
    {
        virtual ~ILevelInfo() = 0;
        virtual void render() = 0;
        virtual void set_level(const std::weak_ptr<ILevel>& level) = 0;

        /// Event raised when the settings button is pressed.
        Event<> on_toggle_settings;
    };
}

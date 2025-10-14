#pragma once

#include <trview.common/Event.h>
#include "../Settings/FontSetting.h"

namespace trview
{
    struct ISettingsWindow
    {
        virtual ~ISettingsWindow() = 0;
        Event<std::string, FontSetting> on_font;
        Event<bool> on_linear_filtering;

        virtual void render() = 0;
        /// <summary>
        /// Toggle the visibility of the settings window.
        /// </summary>
        virtual void toggle_visibility() = 0;
    };
}

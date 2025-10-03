#pragma once

#include <trview.common/Event.h>
#include "../UI/MapColours.h"
#include "../Settings/FontSetting.h"
#include "../Settings/UserSettings.h"

namespace trview
{
    struct ISettingsWindow
    {
        virtual ~ISettingsWindow() = 0;
        Event<std::string, FontSetting> on_font;
        Event<bool> on_linear_filtering;
        Event<UserSettings> on_settings;

        virtual void render() = 0;
        /// <summary>
        /// Toggle the visibility of the settings window.
        /// </summary>
        virtual void toggle_visibility() = 0;
        virtual void set_settings(const UserSettings& settings) = 0;
    };
}

#pragma once

#include "UserSettings.h"

namespace trview
{
    struct ISettingsLoader
    {
        virtual ~ISettingsLoader() = 0;
        virtual UserSettings load_user_settings() const = 0;
        virtual void save_user_settings(const UserSettings& settings) = 0;
    };
}

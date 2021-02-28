#pragma once

#include "ISettingsLoader.h"

namespace trview
{
    class SettingsLoader : public ISettingsLoader
    {
    public:
        virtual ~SettingsLoader() = default;
        virtual UserSettings load_user_settings() const override;
        virtual void save_user_settings(const UserSettings& settings) override;
    };
}

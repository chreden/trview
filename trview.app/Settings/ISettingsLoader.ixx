export module trview.app:ISettingsLoader;

import :UserSettings;

namespace trview
{
    export struct ISettingsLoader
    {
        virtual ~ISettingsLoader() = 0;
        virtual UserSettings load_user_settings() const = 0;
        virtual void save_user_settings(const UserSettings& settings) = 0;
    };
}

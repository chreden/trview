export module trview.app:SettingsLoader;

import std;

import trview.common;

import :ISettingsLoader;

namespace trview
{
    export class SettingsLoader : public ISettingsLoader
    {
    public:
        virtual ~SettingsLoader() = default;
        explicit SettingsLoader(const std::shared_ptr<IFiles>& files);
        UserSettings load_user_settings() const override;
        void save_user_settings(const UserSettings& settings) override;
    private:
        std::shared_ptr<IFiles> _files;
    };
}

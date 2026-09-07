export module trview.app:UserSettingsPatches;

import :UserSettings;

namespace trview
{
    export void patch_settings_load(UserSettings& settings);
    export void patch_settings_save(UserSettings& settings);
}

#pragma once

namespace trview
{
    struct UserSettings;

    void patch_settings_load(UserSettings& settings);
    void patch_settings_save(UserSettings& settings);
}

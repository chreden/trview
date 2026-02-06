#include "UserSettingsPatches.h"
#include "UserSettings.h"

namespace trview
{
    void filter_columns_replacement(UserSettings& settings);

    void patch_settings(UserSettings& settings)
    {
        filter_columns_replacement(settings);
    }

    /// <summary>
    /// Certain default filter columns were replaced by filterable property versions
    /// so columns need to be remapped to their equivalents.
    /// Change after 2.8.0 (2.8.0 was last version without version in settings)
    /// </summary>
    void filter_columns_replacement(UserSettings& settings)
    {
        if (!settings.version.empty())
        {
            return;
        }

        std::ranges::replace(settings.camera_sink_window_columns, "Room", "Room #");
        std::ranges::replace(settings.flyby_columns, "Room", "Room #");
        std::ranges::replace(settings.items_window_columns, "Room", "Room #");
        std::ranges::replace(settings.lights_window_columns, "Room", "Room #");
        std::ranges::replace(settings.triggers_window_columns, "Room", "Room #");
        std::ranges::replace(settings.rooms_window_columns, "Alternate", "Alternate #");
        std::ranges::replace(settings.rooms_window_columns, "Items", "Items #");
        std::ranges::replace(settings.rooms_window_columns, "Triggers", "Triggers #");
        std::ranges::replace(settings.rooms_window_columns, "Statics", "Statics #");
    }
}

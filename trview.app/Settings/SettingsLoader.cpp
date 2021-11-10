#include "SettingsLoader.h"
#include <trview.common/Json.h>
#include <trview.common/Strings.h>

namespace trview
{
    SettingsLoader::SettingsLoader(const std::shared_ptr<IFiles>& files)
        : _files(files)
    {
    }

    UserSettings SettingsLoader::load_user_settings() const
    {
        UserSettings settings;

        try
        {
            auto data = _files->load_file(_files->appdata_directory() + "\\trview\\settings.txt");
            if (!data.has_value())
            {
                return settings;
            }

            auto json = nlohmann::json::parse(data.value().begin(), data.value().end());
            read_attribute(json, settings.camera_sensitivity, "camera");
            read_attribute(json, settings.camera_movement_speed, "movement");
            read_attribute(json, settings.vsync, "vsync");
            read_attribute(json, settings.go_to_lara, "gotolara");
            read_attribute(json, settings.invert_map_controls, "invertmapcontrols");
            read_attribute(json, settings.items_startup, "itemsstartup");
            read_attribute(json, settings.triggers_startup, "triggersstartup");
            read_attribute(json, settings.auto_orbit, "autoorbit");
            read_attribute(json, settings.recent_files, "recent");
            read_attribute(json, settings.invert_vertical_pan, "invertverticalpan");
            read_attribute(json, settings.background_colour, "background");
            read_attribute(json, settings.rooms_startup, "roomsstartup");
            read_attribute(json, settings.camera_acceleration, "cameraacceleration");
            read_attribute(json, settings.camera_acceleration_rate, "cameraaccelerationrate");
            read_attribute(json, settings.camera_display_degrees, "cameradisplaydegrees");
            read_attribute(json, settings.randomizer_tools, "randomizertools");
            read_attribute(json, settings.max_recent_files, "maxrecentfiles");
        }
        catch (...)
        {
            // Nowhere to log this yet...
        }

        return settings;
    }

    void SettingsLoader::save_user_settings(const UserSettings& settings)
    {
        const auto directory = _files->appdata_directory() + "\\trview";
        _files->create_directory(directory);
        const auto file_path = directory + "\\settings.txt";

        try
        {
            nlohmann::json json;
            json["camera"] = settings.camera_sensitivity;
            json["movement"] = settings.camera_movement_speed;
            json["vsync"] = settings.vsync;
            json["gotolara"] = settings.go_to_lara;
            json["invertmapcontrols"] = settings.invert_map_controls;
            json["itemsstartup"] = settings.items_startup;
            json["triggersstartup"] = settings.triggers_startup;
            json["autoorbit"] = settings.auto_orbit;
            json["recent"] = settings.recent_files;
            json["invertverticalpan"] = settings.invert_vertical_pan;
            json["background"] = settings.background_colour;
            json["roomsstartup"] = settings.rooms_startup;
            json["cameraacceleration"] = settings.camera_acceleration;
            json["cameraaccelerationrate"] = settings.camera_acceleration_rate;
            json["cameradisplaydegrees"] = settings.camera_display_degrees;
            json["randomizertools"] = settings.randomizer_tools;
            json["maxrecentfiles"] = settings.max_recent_files;
            _files->save_file(file_path, json.dump());
        }
        catch (...)
        {
        }
    }
}

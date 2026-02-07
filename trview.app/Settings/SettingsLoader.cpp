#include "SettingsLoader.h"
#include <trview.common/Json.h>
#include <trview.common/Strings.h>
#include <trview.common/JsonSerializers.h>
#include <trview.common/Version.h>
#include "UserSettingsPatches.h"

namespace trview
{
    ISettingsLoader::~ISettingsLoader()
    {
    }

    void to_json(nlohmann::json& json, const UserSettings::RecentRoute& item)
    {
        json["route_path"] = item.route_path;
        json["is_rando"] = item.is_rando;
    }

    void to_json(nlohmann::json& json, const UserSettings::WindowPlacement& placement)
    {
        json["min_x"] = placement.min_x;
        json["min_y"] = placement.min_y;
        json["max_x"] = placement.max_x;
        json["max_y"] = placement.max_y;
        json["normal_left"] = placement.normal_left;
        json["normal_top"] = placement.normal_top;
        json["normal_right"] = placement.normal_right;
        json["normal_bottom"] = placement.normal_bottom;
    }

    void to_json(nlohmann::json& json, const FontSetting& setting)
    {
        json["name"] = setting.name;
        json["filename"] = setting.filename;
        json["size"] = setting.size;
    }

    void to_json(nlohmann::json& json, const PluginSetting& setting)
    {
        json["enabled"] = setting.enabled;
    }

    void from_json(const nlohmann::json& json, UserSettings::RecentRoute& item)
    {
        item.route_path = read_attribute<std::string>(json, "route_path");
        item.is_rando = read_attribute<bool>(json, "is_rando");
    }

    void from_json(const nlohmann::json& json, UserSettings::WindowPlacement& placement)
    {
        placement.min_x = read_attribute<int32_t>(json, "min_x");
        placement.min_y = read_attribute<int32_t>(json, "min_y");
        placement.max_x = read_attribute<int32_t>(json, "max_x");
        placement.max_y = read_attribute<int32_t>(json, "max_y");
        placement.normal_left = read_attribute<int32_t>(json, "normal_left");
        placement.normal_top = read_attribute<int32_t>(json, "normal_top");
        placement.normal_right = read_attribute<int32_t>(json, "normal_right");
        placement.normal_bottom = read_attribute<int32_t>(json, "normal_bottom");
    }

    void from_json(const nlohmann::json& json, FontSetting& setting)
    {
        setting.name = read_attribute<std::string>(json, "name");
        setting.filename = read_attribute<std::string>(json, "filename");
        setting.size = read_attribute<int>(json, "size");
    }

    void from_json(const nlohmann::json& json, PluginSetting& setting)
    {
        setting.enabled = read_attribute<bool>(json, "enabled");
    }

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

            auto json = nlohmann::json::parse(data.value().begin(), data.value().end(), nullptr, true, true, true);
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
            read_attribute(json, settings.map_colours, "mapcolours");
            read_attribute(json, settings.route_colour, "routecolour");
            read_attribute(json, settings.waypoint_colour, "waypointcolour");
            read_attribute(json, settings.route_startup, "routestartup");
            read_attribute(json, settings.recent_routes, "recentroutes");
            read_attribute(json, settings.fov, "fov");
            read_attribute(json, settings.camera_sink_startup, "camera_sink_startup");
            read_attribute(json, settings.window_placement, "window_placement");
            read_attribute(json, settings.plugin_directories, "plugin_directories");
            read_attribute(json, settings.toggles, "toggles");
            read_attribute(json, settings.fonts, "fonts");
            read_attribute(json, settings.statics_startup, "statics_startup");
            read_attribute(json, settings.camera_position_window, "camera_position_window");
            read_attribute(json, settings.recent_diff_files, "recent_diff");
            read_attribute(json, settings.plugins, "plugins");
            read_attribute(json, settings.level_sorting_mode, "level_sorting_mode");
            read_attribute(json, settings.items_window_columns, "items_window_columns");
            read_attribute(json, settings.rooms_window_columns, "rooms_window_columns");
            read_attribute(json, settings.statics_window_columns, "statics_window_columns");
            read_attribute(json, settings.sounds_window_columns, "sounds_window_columns");
            read_attribute(json, settings.lights_window_columns, "lights_window_columns");
            read_attribute(json, settings.camera_sink_window_columns, "camera_sink_window_columns");
            read_attribute(json, settings.triggers_window_columns, "triggers_window_columns");
            read_attribute(json, settings.flyby_columns, "flyby_columns");
            read_attribute(json, settings.flyby_node_columns, "flyby_node_columns");
            read_attribute(json, settings.linear_filtering, "linear_filtering");
            read_attribute(json, settings.version, "version");

            settings.recent_files.resize(std::min<std::size_t>(settings.recent_files.size(), settings.max_recent_files));
        }
        catch (...)
        {
            // Nowhere to log this yet...
        }

        try
        {
            auto data = _files->load_file(_files->appdata_directory() + "\\trview\\randomizer.json");
            if (data.has_value())
            {
                auto json = nlohmann::json::parse(data.value().begin(), data.value().end(), nullptr, true, true, true);
                from_json(json, settings.randomizer);
            }
        }
        catch (...)
        {
            // Nowhere to log this yet...
        }

        patch_settings_load(settings);
        return settings;
    }

    void SettingsLoader::save_user_settings(const UserSettings& settings_to_save)
    {
        const auto directory = _files->appdata_directory() + "\\trview";
        _files->create_directory(directory);
        const auto file_path = directory + "\\settings.txt";

        UserSettings settings = settings_to_save;
        patch_settings_save(settings);

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
            json["recent"] = std::list<std::string>(settings.recent_files.begin(), std::next(settings.recent_files.begin(), std::min<std::size_t>(settings.recent_files.size(), settings.max_recent_files)));
            json["invertverticalpan"] = settings.invert_vertical_pan;
            json["background"] = settings.background_colour;
            json["roomsstartup"] = settings.rooms_startup;
            json["cameraacceleration"] = settings.camera_acceleration;
            json["cameraaccelerationrate"] = settings.camera_acceleration_rate;
            json["cameradisplaydegrees"] = settings.camera_display_degrees;
            json["randomizertools"] = settings.randomizer_tools;
            json["maxrecentfiles"] = settings.max_recent_files;
            json["mapcolours"] = settings.map_colours;
            json["routecolour"] = settings.route_colour;
            json["waypointcolour"] = settings.waypoint_colour;
            json["routestartup"] = settings.route_startup;
            json["recentroutes"] = settings.recent_routes;
            json["fov"] = settings.fov;
            json["camera_sink_startup"] = settings.camera_sink_startup;
            if (settings.window_placement)
            {
                json["window_placement"] = settings.window_placement.value();
            }
            json["plugin_directories"] = settings.plugin_directories;
            json["toggles"] = settings.toggles;
            json["fonts"] = settings.fonts;
            json["statics_startup"] = settings.statics_startup;
            json["camera_position_window"] = settings.camera_position_window;
            json["recent_diff"] = std::list<std::string>(settings.recent_diff_files.begin(), std::next(settings.recent_diff_files.begin(), std::min<std::size_t>(settings.recent_diff_files.size(), settings.max_recent_files)));
            json["plugins"] = settings.plugins;
            json["level_sorting_mode"] = settings.level_sorting_mode;
            json["items_window_columns"] = settings.items_window_columns;
            json["rooms_window_columns"] = settings.rooms_window_columns;
            json["statics_window_columns"] = settings.statics_window_columns;
            json["sounds_window_columns"] = settings.sounds_window_columns;
            json["lights_window_columns"] = settings.lights_window_columns;
            json["camera_sink_window_columns"] = settings.camera_sink_window_columns;
            json["triggers_window_columns"] = settings.triggers_window_columns;
            json["flyby_columns"] = settings.flyby_columns;
            json["flyby_node_columns"] = settings.flyby_node_columns;
            json["linear_filtering"] = settings.linear_filtering;
            json["version"] = trview::version();
            _files->save_file(file_path, json.dump());
        }
        catch (...)
        {
        }
    }
}

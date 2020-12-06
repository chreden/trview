#include "UserSettings.h"

namespace trview
{
    namespace
    {
        const int MaxRecentFiles = 10;

        struct SafePath
        {
            wchar_t* path;
            ~SafePath()
            {
                if (path)
                {
                    CoTaskMemFree(path);
                }
            }
        };

        template <typename T>
        void read_setting(const nlohmann::json& json, T& destination, const std::string& attribute_name)
        {
            if (json.count(attribute_name) != 0)
            {
                destination = json[attribute_name].get<T>();
            }
        }
    }

    void UserSettings::add_recent_file(const std::string& file)
    {
        // If the file already exists in the recent files list, remove it from where it is
        // and move it to the to avoid duplicates and to make it ordered by most recent.
        auto existing = std::find(recent_files.begin(), recent_files.end(), file);
        if (existing != recent_files.end())
        {
            recent_files.erase(existing);
        }

        recent_files.push_front(file);
        if (recent_files.size() > MaxRecentFiles)
        {
            recent_files.pop_back();
        }
    }

    UserSettings load_user_settings()
    {
        UserSettings settings;

        SafePath path;
        if (S_OK != SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path.path))
        {
            return settings;
        }

        std::wstring file_path(path.path);
        file_path += L"\\trview\\settings.txt";
        
        try
        {
            std::ifstream file(file_path);
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit | std::ifstream::eofbit);
            if (!file.is_open())
            {
                return settings;
            }

            nlohmann::json json;
            file >> json;

            read_setting(json, settings.camera_sensitivity, "camera");
            read_setting(json, settings.camera_movement_speed, "movement");
            read_setting(json, settings.vsync, "vsync");
            read_setting(json, settings.go_to_lara, "gotolara");
            read_setting(json, settings.invert_map_controls, "invertmapcontrols");
            read_setting(json, settings.items_startup, "itemsstartup");
            read_setting(json, settings.triggers_startup, "triggersstartup");
            read_setting(json, settings.auto_orbit, "autoorbit");
            read_setting(json, settings.recent_files, "recent");
            read_setting(json, settings.invert_vertical_pan, "invertverticalpan");
            read_setting(json, settings.background_colour, "background");
            read_setting(json, settings.rooms_startup, "roomsstartup");
            read_setting(json, settings.camera_acceleration, "cameraacceleration");
            read_setting(json, settings.camera_acceleration_rate, "cameraaccelerationrate");
        }
        catch (...)
        {
            // Nowhere to log this yet...
        }

        return settings;
    }

    void save_user_settings(const UserSettings& settings)
    {
        SafePath path;
        if (S_OK != SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &path.path))
        {
            return;
        }

        std::wstring file_path(path.path);
        file_path += L"\\trview";
        if (!CreateDirectory(file_path.c_str(), nullptr) && GetLastError() != ERROR_ALREADY_EXISTS)
        {
            return;
        }

        file_path += L"\\settings.txt";

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

            std::ofstream file(file_path);
            file << json;
        }
        catch (...)
        {
        }
    }
}

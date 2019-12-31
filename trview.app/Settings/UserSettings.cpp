#include "UserSettings.h"

#include <ShlObj.h>
#include <fstream>
#include <algorithm>
#pragma warning(push)
#pragma warning(disable : 4127)
#include <external/nlohmann/json.hpp>
#pragma warning(pop)

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

            settings.camera_sensitivity = json["camera"].get<float>();
            settings.camera_movement_speed = json["movement"].get<float>();
            settings.vsync = json["vsync"].get<bool>();
            settings.go_to_lara = json["gotolara"].get<bool>();
            settings.invert_map_controls = json["invertmapcontrols"].get<bool>();
            settings.items_startup = json["itemsstartup"].get<bool>();
            settings.triggers_startup = json["triggersstartup"].get<bool>();
            settings.auto_orbit = json["autoorbit"].get<bool>();
            settings.recent_files = json["recent"].get<std::list<std::string>>();
            settings.invert_vertical_pan = json["invertverticalpan"].get<bool>();
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

            std::ofstream file(file_path);
            file << json;
        }
        catch (...)
        {
        }
    }
}

#include "stdafx.h"
#include "UserSettings.h"

#include <ShlObj.h>
#include <fstream>
#include <algorithm>

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

    void UserSettings::add_recent_file(const std::wstring& file)
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
            std::wifstream file(file_path);
            file.exceptions(std::wifstream::failbit | std::wifstream::badbit | std::wifstream::eofbit);
            if (!file.is_open())
            {
                return settings;
            }

            while (!file.eof())
            {
                std::wstring setting;
                file >> setting;

                if (setting == L"camera")
                {
                    file >> settings.camera_sensitivity;
                }
                else if (setting == L"movement")
                {
                    file >> settings.camera_movement_speed;
                }
                else if (setting == L"recent")
                {
                    uint32_t recent_count = 0;
                    file >> recent_count;

                    // Get the eol for the count.
                    std::wstring line;
                    std::getline(file, line);

                    for (uint32_t i = 0; i < recent_count; ++i)
                    {
                        std::wstring recent_file;
                        std::getline(file, recent_file);
                        settings.recent_files.push_back(recent_file);
                    }
                }
            }
        }
        catch (std::ifstream::failure&)
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
        std::wofstream file(file_path);
        if (!file.is_open())
        {
            return;
        }

        file << L"camera "  << settings.camera_sensitivity      << '\n';
        file << L"movement "<< settings.camera_movement_speed   << '\n';
        file << L"recent "  << settings.recent_files.size()     << '\n';
        for (const auto& file_name : settings.recent_files)
        {
            file << file_name << '\n';
        }
    }
}

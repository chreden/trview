#include "stdafx.h"
#include "UserSettings.h"

#include <ShlObj.h>
#include <PathCch.h>
#include <fstream>

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
                if (path != nullptr)
                {
                    CoTaskMemFree(path);
                }
            }
        };
    }

    void UserSettings::add_recent_file(const std::wstring& file)
    {
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

        wchar_t file_path[PATHCCH_MAX_CCH];
        memset(file_path, 0, sizeof(file_path));

        if (S_OK != PathCchCombine(file_path, PATHCCH_MAX_CCH, path.path, L"trview\\settings.txt"))
        {
            return settings;
        }

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
                else if (setting == L"recent")
                {
                    uint32_t recent_count = 0;
                    file >> recent_count;

                    // Get the eol for the count.
                    std::getline(file, std::wstring());

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

        wchar_t file_path[PATHCCH_MAX_CCH];
        memset(file_path, 0, sizeof(file_path));
        if (S_OK != PathCchCombine(file_path, PATHCCH_MAX_CCH, path.path, L"trview"))
        {
            return;
        }

        if (!CreateDirectory(file_path, nullptr))
        {
            return;
        }

        if (S_OK != PathCchCombine(file_path, PATHCCH_MAX_CCH, file_path, L"settings.txt"))
        {
            return;
        }

        std::wofstream file(file_path);
        if (!file.is_open())
        {
            return;
        }

        file << L"camera " << settings.camera_sensitivity << '\n';
        file << L"recent " << settings.recent_files.size() << '\n';
        for (const auto& file_name : settings.recent_files)
        {
            file << file_name << '\n';
        }
    }
}

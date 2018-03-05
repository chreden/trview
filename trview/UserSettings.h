#pragma once

#include <list>
#include <string>

namespace trview
{
    struct UserSettings
    {
        void add_recent_file(const std::wstring& file);

        std::list<std::wstring> recent_files;
        float                   camera_sensitivity;
    };
}


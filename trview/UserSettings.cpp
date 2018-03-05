#include "stdafx.h"
#include "UserSettings.h"

namespace trview
{
    namespace
    {
        const int MaxRecentFiles = 10;
    }

    void UserSettings::add_recent_file(const std::wstring& file)
    {
        recent_files.push_front(file);
        if (recent_files.size() > MaxRecentFiles)
        {
            recent_files.pop_back();
        }
    }
}

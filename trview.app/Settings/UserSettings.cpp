#include "UserSettings.h"

namespace trview
{
    namespace
    {
        const int MaxRecentFiles = 10;
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
}

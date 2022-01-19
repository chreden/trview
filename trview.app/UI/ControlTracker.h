#pragma once

#include <unordered_map>

namespace trview
{
    class ControlTracker
    {
    public:
        struct Entry
        {
            float x, y, width, height;
        };
        void track(const std::string& name);
        std::optional<Entry> find_tracked_control(const std::string& name) const;
    private:
        std::unordered_map<std::string, Entry> _tracked_controls;
    };
}

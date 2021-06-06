#pragma once

#include <optional>
#include <string>
#include <trlevel/LevelVersion.h>

namespace trview
{
    class Action final
    {
    public:
        std::string name() const;
        std::string display_name() const;
        bool is_available(trlevel::LevelVersion version) const;
        std::optional<float> time(trlevel::LevelVersion version) const;
    private:
        std::string _name;
        std::string _display_name;
        std::unordered_map<trlevel::LevelVersion, float> _times;
    };
}

#include "Action.h"

namespace trview
{
    std::string Action::name() const
    {
        return _name;
    }

    std::string Action::display_name() const
    {
        return _display_name;
    }

    bool Action::is_available(trlevel::LevelVersion version) const
    {
        return _times.find(version) != _times.end();
    }

    std::optional<float> Action::time(trlevel::LevelVersion version) const
    {
        const auto found = _times.find(version);
        if (found == _times.end())
        {
            return {};
        }
        return found->second;
    }
}

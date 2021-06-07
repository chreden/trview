#include "Actions.h"
#include <trview.common/Json.h>

namespace trview
{
    Actions::Actions(const std::string& actions_json)
    {
        auto json = nlohmann::json::parse(actions_json.begin(), actions_json.end());
        for (const auto& action : json["actions"])
        {
            auto name = read_attribute<std::string>(action, "name");
            auto display_name = read_attribute<std::string>(action, "display_name");

            if (!action.count("times"))
            {
                continue;
            }
            
            std::unordered_map<trlevel::LevelVersion, float> times;
            auto times_json = action["times"];

            const auto get_version_time = [&](trlevel::LevelVersion version, const std::string& name)
            {
                if (times_json.count(name))
                {
                    times.insert({ version, times_json[name].get<float>() });
                }
            };

            get_version_time(trlevel::LevelVersion::Tomb1, "tr1");
            get_version_time(trlevel::LevelVersion::Tomb2, "tr2");
            get_version_time(trlevel::LevelVersion::Tomb3, "tr3");
            get_version_time(trlevel::LevelVersion::Tomb4, "tr4");
            get_version_time(trlevel::LevelVersion::Tomb5, "tr5");

            _actions.insert({ name, Action(name, display_name, times) });
        }
    }

    std::vector<Action> Actions::actions(trlevel::LevelVersion version) const
    {
        std::vector<Action> results;
        for (const auto& action : _actions)
        {
            if (action.second.is_available(version))
            {
                results.push_back(action.second);
            }
        }
        return results;
    }

    std::optional<Action> Actions::action(const std::string& name) const
    {
        auto result = _actions.find(name);
        if (result == _actions.end())
        {
            return {};
        }
        return result->second;
    }
}

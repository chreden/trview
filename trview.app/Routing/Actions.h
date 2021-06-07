#pragma once

#include "IActions.h"
#include "Action.h"

namespace trview
{
    class Actions final : public IActions
    {
    public:
        explicit Actions(const std::string& actions_json);
        virtual ~Actions() = default;
        virtual std::vector<Action> actions(trlevel::LevelVersion version) const override;
        virtual std::optional<Action> action(const std::string& name) const override;
    private:
        std::unordered_map<std::string, Action> _actions;
    };
}

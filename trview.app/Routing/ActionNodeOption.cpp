#include "ActionNodeOption.h"

namespace trview
{
    namespace
    {
        std::unordered_map<Action, std::string> action_textures
        {
            { Action::Run, "action_run" },
            { Action::Walk, "action_walk" },
            { Action::Sprint, "action_sprint" }
        };
    }

    ActionNodeOption::ActionNodeOption(const std::vector<TransparentTriangle>& triangles)
        : _triangles(triangles)
    {
    }

    void ActionNodeOption::render(DirectX::SimpleMath::Matrix& transform, TransparencyBuffer& transparency, Action action)
    {
        for (const auto& triangle : _triangles)
        {
            transparency.add(triangle.transform(transform, action_textures[action]));
        }
    }
}

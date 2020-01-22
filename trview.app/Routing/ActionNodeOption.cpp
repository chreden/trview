#include "ActionNodeOption.h"

namespace trview
{
    ActionNodeOption::ActionNodeOption(const std::vector<TransparentTriangle>& triangles)
        : _triangles(triangles)
    {
    }

    void ActionNodeOption::render(DirectX::SimpleMath::Matrix& transform, TransparencyBuffer& transparency)
    {
        for (const auto& triangle : _triangles)
        {
            transparency.add(triangle.transform(transform, "action_run"));
        }
    }
}

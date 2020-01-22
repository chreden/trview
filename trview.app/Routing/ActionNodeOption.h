#pragma once

#include <trview.app/Geometry/TransparencyBuffer.h>
#include <vector>

namespace trview
{
    class ActionNodeOption
    {
    public:
        explicit ActionNodeOption(const std::vector<TransparentTriangle>& triangles);

        void render(DirectX::SimpleMath::Matrix& transform, TransparencyBuffer& transparency);
    private:
        std::vector<TransparentTriangle> _triangles;
    };
}

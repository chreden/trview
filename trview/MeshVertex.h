#pragma once

#include <SimpleMath.h>

namespace trview
{
    struct MeshVertex
    {
        DirectX::SimpleMath::Vector3 pos;
        DirectX::SimpleMath::Vector2 uv;
        DirectX::SimpleMath::Color colour;
    };
}

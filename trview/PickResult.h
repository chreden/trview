#pragma once

#include <SimpleMath.h>

namespace trview
{
    struct PickResult
    {
        enum class Type
        {
            Room,
            Entity
        };

        bool                         hit{ false };
        float                        distance;
        DirectX::SimpleMath::Vector3 position;
        Type                         type{ Type::Room };
        uint32_t                     index{ 0u };
    };
}

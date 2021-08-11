#pragma once

import trview.common.size;
#include <trview.common/Point.h>
#include <SimpleMath.h>

namespace trview
{
    struct PickInfo
    {
        Size  screen_size;
        Point screen_position;
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector3 direction;
    };
}

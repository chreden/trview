#pragma once

import trview.common.size;
import trview.common.point;
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

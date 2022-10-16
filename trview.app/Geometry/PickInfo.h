#pragma once

#include <SimpleMath.h>

import trview.common;

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

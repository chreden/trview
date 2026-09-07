module;

#include <SimpleMath.h>

export module trview.app:PickInfo;

import trview.common;

namespace trview
{
    export struct PickInfo
    {
        Size  screen_size;
        Point screen_position;
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Vector3 direction;
    };
}

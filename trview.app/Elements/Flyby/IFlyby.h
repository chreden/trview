#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <trlevel/trtypes.h>

#include "../../Geometry/IRenderable.h"

namespace trview
{
    struct IFlyby : public IRenderable
    {
        using Source = std::function<std::shared_ptr<IFlyby>(const std::vector<trlevel::tr4_flyby_camera>&)>;

        virtual ~IFlyby() = 0;
        virtual DirectX::SimpleMath::Vector3 position_at(float value) const = 0;
    };
}

#pragma once

#include <trlevel/trtypes.h>
#include <functional>

namespace trview
{
    struct IFlyby
    {
        using Source = std::function<std::shared_ptr<IFlyby>(const std::vector<trlevel::tr4_flyby_camera>&)>;

        virtual ~IFlyby() = 0;
    };
}

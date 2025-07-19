#pragma once

#include <cstdint>
#include <functional>

#include <SimpleMath.h>

#include <trlevel/trtypes.h>

namespace trview
{
    struct IFlybyNode
    {
        using Source = std::function<std::shared_ptr<IFlybyNode>(const trlevel::tr4_flyby_camera&)>;

        virtual ~IFlybyNode() = 0;
        virtual DirectX::SimpleMath::Vector3 direction() const = 0;
        virtual uint16_t flags() const = 0;
        virtual uint16_t fov() const = 0;
        virtual int32_t number() const = 0;
        virtual DirectX::SimpleMath::Vector3 position() const = 0;
        virtual int16_t roll() const = 0;
        virtual uint32_t room() const = 0;
        virtual uint16_t speed() const = 0;
        virtual uint16_t timer() const = 0;
    };
}

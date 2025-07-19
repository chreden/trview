#pragma once

#include "IFlybyNode.h"

namespace trview
{
    class FlybyNode final : public IFlybyNode
    {
    public:
        explicit FlybyNode(const trlevel::tr4_flyby_camera& camera);
        virtual ~FlybyNode() = default;
        DirectX::SimpleMath::Vector3 direction() const override;
        uint16_t flags() const override;
        uint16_t fov() const override;
        DirectX::SimpleMath::Vector3 position() const override;
        int16_t roll() const override;
        uint32_t room() const override;
        uint16_t speed() const override;
        uint16_t timer() const override;
    private:
        DirectX::SimpleMath::Vector3 _direction;
        uint16_t _flags{ 0 };
        uint16_t _fov{ 0 };
        DirectX::SimpleMath::Vector3 _position;
        int16_t _roll{ 0 };
        uint32_t _room{ 0 };
        uint16_t _speed{ 0 };
        uint16_t _timer{ 0 };
    };
}

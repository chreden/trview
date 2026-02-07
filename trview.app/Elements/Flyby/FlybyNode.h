#pragma once

#include "IFlybyNode.h"

namespace trview
{
    class FlybyNode final : public IFlybyNode
    {
    public:
        explicit FlybyNode(const trlevel::tr4_flyby_camera& camera, const std::weak_ptr<IFlyby>& flyby, const std::weak_ptr<ILevel>& level);
        virtual ~FlybyNode() = default;
        DirectX::SimpleMath::Vector3 direction() const override;
        uint16_t flags() const override;
        std::weak_ptr<IFlyby> flyby() const override;
        uint16_t fov() const override;
        std::weak_ptr<ILevel> level() const override;
        int32_t number() const override;
        DirectX::SimpleMath::Vector3 position() const override;
        int16_t roll() const override;
        uint32_t room() const override;
        uint16_t speed() const override;
        uint16_t timer() const override;
        int32_t filterable_index() const override;
    private:
        std::weak_ptr<ILevel> _level;
        std::weak_ptr<IFlyby> _flyby;
        DirectX::SimpleMath::Vector3 _direction;
        uint16_t _flags{ 0 };
        uint16_t _fov{ 0 };
        int32_t _number{ 0 };
        DirectX::SimpleMath::Vector3 _position;
        int16_t _roll{ 0 };
        uint32_t _room{ 0 };
        uint16_t _speed{ 0 };
        uint16_t _timer{ 0 };
    };
}

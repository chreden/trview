#include "FlybyNode.h"

namespace trview
{
    using namespace trlevel;
    using namespace DirectX::SimpleMath;

    IFlybyNode::~IFlybyNode()
    {
    }

    FlybyNode::FlybyNode(const tr4_flyby_camera& node, const std::weak_ptr<IFlyby>& flyby, const std::weak_ptr<ILevel>& level)
        : _flyby(flyby),
        _level(level),
        _direction(Vector3(static_cast<float>(node.dx), static_cast<float>(node.dy), static_cast<float>(node.dz)) / trlevel::Scale),
        _flags(node.flags),
        _fov(node.fov),
        _number(node.index),
        _position(Vector3(static_cast<float>(node.x), static_cast<float>(node.y), static_cast<float>(node.z)) / trlevel::Scale),
        _roll(node.roll),
        _room(node.room_id),
        _speed(node.speed),
        _timer(node.timer)
    {
    }

    Vector3 FlybyNode::direction() const
    {
        return _direction;
    }

    uint16_t FlybyNode::flags() const
    {
        return _flags;
    }

    std::weak_ptr<IFlyby> FlybyNode::flyby() const
    {
        return _flyby;
    }

    uint16_t FlybyNode::fov() const
    {
        return _fov;
    }

    std::weak_ptr<ILevel> FlybyNode::level() const
    {
        return _level;
    }

    int32_t FlybyNode::number() const
    {
        return _number;
    }

    Vector3 FlybyNode::position() const
    {
        return _position;
    }

    int16_t FlybyNode::roll() const
    {
        return _roll;
    }

    uint32_t FlybyNode::room() const
    {
        return _room;
    }

    uint16_t FlybyNode::speed() const
    {
        return _speed;
    }

    uint16_t FlybyNode::timer() const
    {
        return _timer;
    }

    int32_t FlybyNode::filterable_index() const
    {
        return static_cast<int32_t>(_number);
    }
}

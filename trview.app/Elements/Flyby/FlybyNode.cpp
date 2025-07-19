#include "FlybyNode.h"

namespace trview
{
    using namespace trlevel;
    using namespace DirectX::SimpleMath;

    IFlybyNode::~IFlybyNode()
    {
    }

    FlybyNode::FlybyNode(const tr4_flyby_camera& node)
        : _direction(Vector3(static_cast<float>(node.dx), static_cast<float>(node.dy), static_cast<float>(node.dz)) / trlevel::Scale),
        _flags(node.flags),
        _fov(node.fov),
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

    uint16_t FlybyNode::fov() const
    {
        return _fov;
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
}

#include "stdafx.h"
#include "OrbitCamera.h"

namespace trview
{
    using namespace DirectX::SimpleMath;

    OrbitCamera::OrbitCamera(const Size& size)
        : Camera(size)
    {
    }

    void OrbitCamera::reset()
    {
        set_rotation_yaw(default_yaw);
        set_rotation_pitch(default_pitch);
        set_zoom(default_zoom);
    }

    void OrbitCamera::set_target(const Vector3& target)
    {
        _target = target;
        update_vectors();
    }

    const Vector3& OrbitCamera::target() const
    {
        return _target;
    }

    void OrbitCamera::update_vectors()
    {
        auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
        _position = Vector3::Transform(Vector3(0, 0, -_zoom), rotate) + _target;
        _up = Vector3::Transform(Vector3::Down, rotate);
        (_target - _position).Normalize(_forward);
        calculate_view_matrix();
    }
}
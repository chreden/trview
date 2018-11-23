#include "OrbitCamera.h"
#include <algorithm>

namespace trview
{
    using namespace DirectX::SimpleMath;

    namespace
    {
        const float max_zoom = 100.0f;
        const float min_zoom = 0.1f;
    }

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

    void OrbitCamera::set_zoom(float zoom)
    {
        _zoom = std::min(std::max(zoom, min_zoom), max_zoom);
        update_vectors();
    }

    Vector3 OrbitCamera::target() const
    {
        return _target;
    }

    float OrbitCamera::zoom() const
    {
        return _zoom;
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
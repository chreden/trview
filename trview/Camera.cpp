#include "stdafx.h"
#include "Camera.h"

#include <algorithm>

namespace trview
{
    namespace
    {
        const float max_zoom = 100.0f;
        const float min_zoom = 0.1f;
    }

    Camera::Camera(const Size& size)
    {
        calculate_projection_matrix(size);
    }

    DirectX::SimpleMath::Vector3 Camera::target() const
    {
        return _target;
    }

    float Camera::rotation_pitch() const
    {
        return _rotation_pitch;
    }

    float Camera::rotation_yaw() const
    {
        return _rotation_yaw;
    }

    float Camera::zoom() const
    {
        return _zoom;
    }

    void Camera::calculate_projection_matrix(const Size& size)
    {
        using namespace DirectX;
        _view_size = size;
        float aspect_ratio = size.width / size.height;
        _projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect_ratio, 0.1f, 10000.0f);
        _view_projection = _view * _projection;
    }

    void Camera::set_target(const DirectX::SimpleMath::Vector3& target)
    {
        _target = target;
        calculate_view_matrix();
    }

    void Camera::calculate_view_matrix()
    {
        using namespace DirectX;
        using namespace SimpleMath;

        Vector3 eye_position(0, 0, -_zoom);

        auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
        eye_position = Vector3::Transform(eye_position, rotate) + _target;

        Vector3 up_vector = Vector3::Transform(Vector3(0, 1, 0), rotate);
        _view = XMMatrixLookAtLH(eye_position, _target, up_vector);
        _view_projection = _view * _projection;
    }

    void Camera::set_rotation_pitch(float rotation)
    {
        _rotation_pitch = std::max(-DirectX::XM_PIDIV2, std::min(rotation, DirectX::XM_PIDIV2));
        calculate_view_matrix();
    }

    void Camera::set_rotation_yaw(float rotation)
    {
        _rotation_yaw = rotation;
        calculate_view_matrix();
    }

    void Camera::set_zoom(float zoom)
    {
        _zoom = std::min(std::max(zoom, min_zoom), max_zoom);
        calculate_view_matrix();
    }

    DirectX::SimpleMath::Matrix Camera::view() const
    {
        return _view;
    }

    DirectX::SimpleMath::Matrix Camera::projection() const
    {
        return _projection;
    }

    DirectX::SimpleMath::Matrix Camera::view_projection() const
    {
        return _view_projection;
    }

    void Camera::reset()
    {
        set_rotation_yaw(default_yaw);
        set_rotation_pitch(default_pitch);
        set_zoom(default_zoom);
    }

    DirectX::SimpleMath::Vector3 Camera::position() const
    {
        using namespace DirectX::SimpleMath;
        Vector3 eye_position(0, 0, -_zoom);
        auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
        return Vector3::Transform(eye_position, rotate) + _target;
    }

    DirectX::SimpleMath::Vector3 Camera::up() const
    {
        using namespace DirectX::SimpleMath;
        auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
        return Vector3::Transform(Vector3::Up, rotate);
    }

    DirectX::SimpleMath::Vector3 Camera::forward() const
    {
        using namespace DirectX::SimpleMath;
        auto to = _target - position();
        to.Normalize();
        return to;
    }

    void Camera::set_view_size(const Size& size)
    {
        calculate_projection_matrix(size);
    }

    Size Camera::view_size() const
    {
        return _view_size;
    }
}
#include "Camera.h"
#include <algorithm>

namespace trview
{
    using namespace DirectX::SimpleMath;

    namespace
    {
        const float max_zoom = 100.0f;
        const float min_zoom = 0.1f;
    }

    Camera::Camera(const Size& size)
    {
        calculate_view_matrix();
        calculate_projection_matrix(size);
    }

    Vector3 Camera::target() const
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
        _projection = XMMatrixPerspectiveFovRH(XM_PIDIV4, aspect_ratio, 0.1f, 10000.0f);
        _projection_lh = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect_ratio, 0.1f, 10000.0f);
        _view_projection = _view * _projection;
    }

    void Camera::set_target(const Vector3& target)
    {
        _target = target;
        _position = Vector3::Transform(Vector3(0, 0, -_zoom), Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0)) + _target;
        calculate_view_matrix();
    }

    void Camera::calculate_view_matrix()
    {
        auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
        Vector3 up_vector = Vector3::Transform(Vector3::Down, rotate);
        _view = XMMatrixLookAtRH(_position, _target, up_vector);
        _view_lh = XMMatrixLookAtLH(_position, _target, up_vector);
        _view_projection = _view * _projection;
    }

    void Camera::set_rotation_pitch(float rotation)
    {
        _rotation_pitch = std::max(-DirectX::XM_PIDIV2, std::min(rotation, DirectX::XM_PIDIV2));
        _position = Vector3::Transform(Vector3(0, 0, -_zoom), Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0)) + _target;
        calculate_view_matrix();
    }

    void Camera::set_rotation_yaw(float rotation)
    {
        _rotation_yaw = rotation;
        _position = Vector3::Transform(Vector3(0, 0, -_zoom), Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0)) + _target;
        calculate_view_matrix();
    }

    void Camera::set_zoom(float zoom)
    {
        _zoom = std::min(std::max(zoom, min_zoom), max_zoom);
        _position = Vector3::Transform(Vector3(0, 0, -_zoom), Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0)) + _target;
        calculate_view_matrix();
    }

    Matrix Camera::view() const
    {
        return _view;
    }

    Matrix Camera::projection() const
    {
        return _projection;
    }

    Matrix Camera::view_projection() const
    {
        return _view_projection;
    }

    void Camera::reset()
    {
        set_rotation_yaw(default_yaw);
        set_rotation_pitch(default_pitch);
        set_zoom(default_zoom);
    }

    Vector3 Camera::position() const
    {
        return _position;
    }

    Vector3 Camera::up() const
    {
        auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
        return Vector3::Transform(Vector3::Down, rotate);
    }

    Vector3 Camera::forward() const
    {
        auto to = _target - position();
        to.Normalize();
        return to;
    }

    void Camera::set_view_size(const Size& size)
    {
        _view_size = size;
        calculate_projection_matrix(size);
    }

    Size Camera::view_size() const
    {
        return _view_size;
    }

    DirectX::BoundingFrustum Camera::frustum() const
    {
        using namespace DirectX;
        XMVECTOR determinant;
        XMMATRIX inv_view_lh = XMMatrixInverse(&determinant, _view_lh);
        BoundingFrustum boundingFrustum(_projection_lh);
        boundingFrustum.Transform(boundingFrustum, inv_view_lh);
        return boundingFrustum;
    }
}
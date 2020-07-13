#include "stdafx.h"
#include "Camera.h"

namespace trview
{
    using namespace DirectX::SimpleMath;

    namespace
    {
        const float max_zoom = 100.0f;
        const float min_zoom = 0.1f;

        float angle_to(float current, float target)
        {
            return atan2(sin(target - current), cos(target - current));
        }
    }

    const float Pi = 3.1415926535897932384626433832796f;
    const float Pi2 = 3.1415926535897932384626433832796f * 2.0f;

    Camera::Camera(const Size& size)
        : _view_size(size), _forward(Vector3::Forward), _up(Vector3::Down), _position(Vector3::Zero)
    {
        calculate_view_matrix();
        calculate_projection_matrix();
    }

    Vector3 Camera::forward() const
    {
        return _forward;
    }

    const DirectX::BoundingFrustum& Camera::frustum() const
    {
        return _bounding_frustum;
    }

    Vector3 Camera::position() const
    {
        return _position;
    }

    Vector3 Camera::rendering_position() const
    {
        return _projection_mode == ProjectionMode::Orthographic
            ? _position - _forward * 100
            : _position;
    }

    const Matrix& Camera::projection() const
    {
        return _projection;
    }

    ProjectionMode Camera::projection_mode() const
    {
        return _projection_mode;
    }

    float Camera::rotation_pitch() const
    {
        return _rotation_pitch;
    }

    float Camera::rotation_yaw() const
    {
        return _rotation_yaw;
    }

    void Camera::rotate_to_pitch(float rotation)
    {
        _target_rotation_pitch = rotation;
    }

    void Camera::rotate_to_yaw(float rotation)
    {
        _target_rotation_yaw = rotation;
    }

    void Camera::set_projection_mode(ProjectionMode mode)
    {
        if (mode == ProjectionMode::Perspective && _projection_mode == ProjectionMode::Orthographic)
        {
            _zoom = _ortho_size;
        }
        else if (mode == ProjectionMode::Orthographic && _projection_mode == ProjectionMode::Perspective)
        {
            _ortho_size = _zoom;
        }

        _projection_mode = mode;
        calculate_view_matrix();
        calculate_projection_matrix();
    }

    void Camera::set_rotation_pitch(float rotation)
    {
        _rotation_pitch = std::max(-DirectX::XM_PIDIV2, std::min(rotation, DirectX::XM_PIDIV2));
        _target_rotation_pitch.reset();
        _target_rotation_yaw.reset();
        update_vectors();
    }

    void Camera::set_rotation_yaw(float rotation)
    {
        _rotation_yaw = rotation - static_cast<int>(rotation / Pi2) * Pi2;
        _target_rotation_pitch.reset();
        _target_rotation_yaw.reset();
        update_vectors();
    }

    void Camera::set_view_size(const Size& size)
    {
        _view_size = size;
        calculate_projection_matrix();
    }

    void Camera::set_zoom(float zoom)
    {
        if (_projection_mode == ProjectionMode::Orthographic)
        {
            _ortho_size = std::min(std::max(zoom, 1.0f), 200.0f);
            calculate_projection_matrix();
        }
        else
        {
            _zoom = std::min(std::max(zoom, min_zoom), max_zoom);
            update_vectors();
        }
    }

    Vector3 Camera::up() const
    {
        return _up;
    }

    void Camera::update(float elapsed)
    {
        const float speed = 10.0f;

        if (_target_rotation_yaw.has_value())
        {
            float diff = angle_to(_rotation_yaw, _target_rotation_yaw.value());
            float amount = std::min(speed * elapsed, abs(diff));
            _rotation_yaw += amount * (diff > 0 ? 1 : -1);

            if (abs(diff) < 0.01f)
            {
                _rotation_yaw = _target_rotation_yaw.value();
                _target_rotation_yaw.reset();
            }

            update_vectors();
        }

        if (_target_rotation_pitch.has_value())
        {
            float diff = angle_to(_rotation_pitch, _target_rotation_pitch.value());
            float amount = std::min(speed * elapsed, abs(diff));
            _rotation_pitch += amount * (diff > 0 ? 1 : -1);

            if (abs(diff) < 0.01f)
            {
                _rotation_pitch = _target_rotation_pitch.value();
                _target_rotation_pitch.reset();
            }

            update_vectors();
        }
    }

    const Matrix& Camera::view() const
    {
        return _view;
    }

    const Matrix& Camera::view_projection() const
    {
        return _view_projection;
    }

    const Size& Camera::view_size() const
    {
        return _view_size;
    }

    float Camera::zoom() const
    {
        return _projection_mode == ProjectionMode::Orthographic ? _ortho_size : _zoom;
    }

    void Camera::calculate_bounding_frustum()
    {
        using namespace DirectX;
        XMVECTOR determinant;
        XMMATRIX inv_view_lh = XMMatrixInverse(&determinant, _view_lh);
        BoundingFrustum boundingFrustum(_projection_lh);
        boundingFrustum.Transform(_bounding_frustum, inv_view_lh);
        on_view_changed();
    }

    void Camera::calculate_projection_matrix()
    {
        using namespace DirectX;

        if (_projection_mode == ProjectionMode::Perspective)
        {
            float aspect_ratio = _view_size.width / _view_size.height;
            _projection = XMMatrixPerspectiveFovRH(XM_PIDIV4, aspect_ratio, 0.1f, 10000.0f);
            _projection_lh = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect_ratio, 0.1f, 10000.0f);
        }
        else if (_projection_mode == ProjectionMode::Orthographic)
        {
            auto width = _ortho_size;
            auto height = width * (_view_size.height / _view_size.width);

            _projection = XMMatrixOrthographicRH(width, height, 0.1f, 10000.0f);
            _projection_lh = XMMatrixOrthographicLH(width, height, 0.1f, 10000.0f);
        }

        _view_projection = _view * _projection;
        calculate_bounding_frustum();
    }

    void Camera::calculate_view_matrix()
    {
        if (_projection_mode == ProjectionMode::Orthographic)
        {
            // Scale the position back so that the level doesn't get clipped near the camera.
            _view = XMMatrixLookAtRH(rendering_position(), _position + _forward, _up);
            _view_lh = XMMatrixLookAtLH(rendering_position(), _position + _forward, _up);
        }
        else
        {
            _view = XMMatrixLookAtRH(_position, _position + _forward, _up);
            _view_lh = XMMatrixLookAtLH(_position, _position + _forward, _up);
        }
        _view_projection = _view * _projection;
        calculate_bounding_frustum();
    }
}

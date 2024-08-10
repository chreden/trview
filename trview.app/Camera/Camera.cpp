#include "Camera.h"
#include <trview.common/Maths.h>
#include "Settings/UserSettings.h"

namespace trview
{
    using namespace DirectX::SimpleMath;

    namespace
    {
        const float max_zoom = 100.0f;
        const float min_zoom = 0.1f;
        const float movement_speed_multiplier = 23.0f;

        float angle_to(float current, float target)
        {
            return atan2(sin(target - current), cos(target - current));
        }

        Camera::Alignment camera_mode_to_alignment(ICamera::Mode mode)
        {
            return mode == ICamera::Mode::Axis ? Camera::Alignment::Axis : Camera::Alignment::Camera;
        }
    }

    ICamera::~ICamera()
    {
    }

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

    const DirectX::BoundingFrustum Camera::frustum() const
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

    const Matrix Camera::projection() const
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
        _last_rotation = 0.0f;
        _rotation_pitch = std::max(-DirectX::XM_PIDIV2, std::min(rotation, DirectX::XM_PIDIV2));
        _target_rotation_pitch.reset();
        _target_rotation_yaw.reset();
        update_vectors();
    }

    void Camera::set_rotation_yaw(float rotation)
    {
        _last_rotation = 0.0f;
        _rotation_yaw = rotation - static_cast<int>(rotation / maths::Pi2) * maths::Pi2;
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

    void Camera::update(float elapsed, const DirectX::SimpleMath::Vector3& movement)
    {
        free_update(elapsed, movement);


        const float speed = 10.0f;

        if (_last_rotation.has_value())
        {
            _last_rotation = _last_rotation.value() + elapsed;
        }

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

    const Matrix Camera::view() const
    {
        return _view;
    }

    const Matrix Camera::view_projection() const
    {
        return _view_projection;
    }

    const Size Camera::view_size() const
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
            const float fov_rad = DirectX::XMConvertToRadians(_fov);
            _projection = XMMatrixPerspectiveFovRH(fov_rad, aspect_ratio, 0.1f, 10000.0f);
            _projection_lh = XMMatrixPerspectiveFovLH(fov_rad, aspect_ratio, 0.1f, 10000.0f);
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

    bool Camera::idle_rotation() const
    {
        return !_last_rotation.has_value() || _last_rotation.value() > 0.3f;
    }

    void Camera::set_fov(float value)
    {
        _fov = value;
        calculate_projection_matrix();
    }

    void Camera::set_settings(const UserSettings& settings)
    {
        set_fov(settings.fov);

        _acceleration_enabled = settings.camera_acceleration;;
        _acceleration_rate = settings.camera_acceleration_rate;

        if (!_acceleration_enabled)
        {
            _acceleration = 1.0f;
        }
    }

    void Camera::move(const Vector3& movement, float elapsed)
    {
        update_acceleration(movement, elapsed);

        // Scale the movement by elapsed time to keep it framerate independent - also apply
        // camera movement acceleration if present.
        auto scaled_movement = movement * elapsed * _acceleration;

        if (_projection_mode == ProjectionMode::Orthographic)
        {
            auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
            _position += Vector3::Transform(Vector3(scaled_movement.x, -scaled_movement.z, 0), rotate);
        }
        else
        {
            if (_alignment == Alignment::Camera)
            {
                auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
                _position += Vector3(0, scaled_movement.y, 0) + Vector3::Transform(Vector3(scaled_movement.x, 0, scaled_movement.z), rotate);
            }
            else if (_alignment == Alignment::Axis)
            {
                _position += scaled_movement;
            }
        }

        if (scaled_movement.LengthSquared() > 0)
        {
            calculate_view_matrix();
        }
    }

    void Camera::update_acceleration(const Vector3& movement, float elapsed)
    {
        if (!_acceleration_enabled)
        {
            _acceleration = 1.0f;
        }
        else if (movement.LengthSquared() == 0)
        {
            _acceleration = 0.0f;
        }
        else
        {
            _acceleration = std::min(_acceleration + std::max(_acceleration_rate, 0.1f) * elapsed, 1.0f);
        }
    }

    void Camera::free_update(float elapsed, const DirectX::SimpleMath::Vector3& movement)
    {
        if (_mode == Mode::Free || _mode == Mode::Axis)
        {
            const float Speed = std::max(0.01f, _movement_speed) * movement_speed_multiplier;
            move(movement * Speed, elapsed);
        }
    }

    void Camera::update_vectors()
    {
        if (_mode == Mode::Orbit)
        {
            auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
            _position = Vector3::Transform(Vector3(0, 0, -_zoom), rotate) + _target;
            _up = Vector3::Transform(Vector3::Down, rotate);
            (_target - _position).Normalize(_forward);
            calculate_view_matrix();
        }
        else if (_mode == Mode::Free || _mode == Mode::Axis)
        {
            const auto rotation = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
            _forward = Vector3::Transform(Vector3::Backward, rotation);
            _up = Vector3::Transform(Vector3::Down, rotation);
            calculate_view_matrix();
        }
    }

    void Camera::set_target(const Vector3& target)
    {
        _target = target;
        update_vectors();
    }

    void Camera::set_position(const DirectX::SimpleMath::Vector3& position)
    {
        _position = position;
        calculate_view_matrix();
    }

    void Camera::set_mode(Mode mode)
    {
        _mode = mode;
        _alignment = camera_mode_to_alignment(mode);
    }

    void Camera::reset()
    {
        set_rotation_yaw(default_yaw);
        set_rotation_pitch(default_pitch);
        set_zoom(default_zoom);
        _last_rotation.reset();
    }

    DirectX::SimpleMath::Vector3 Camera::target() const
    {
        return _target;
    }
}

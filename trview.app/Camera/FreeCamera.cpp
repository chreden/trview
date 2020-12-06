#include "FreeCamera.h"

namespace trview
{
    using namespace DirectX::SimpleMath;

    FreeCamera::FreeCamera(const Size& size)
        : Camera(size)
    {
        _rotation_yaw = 0.0f;
        _rotation_pitch = 0.0f;
    }

    void FreeCamera::move(const Vector3& movement, float elapsed)
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

    void FreeCamera::set_alignment(Alignment alignment)
    {
        _alignment = alignment;
        calculate_projection_matrix();
    }

    void FreeCamera::set_position(const Vector3& position)
    {
        _position = position;
        calculate_view_matrix();
    }

    void FreeCamera::set_acceleration_settings(bool enabled, float rate)
    {
        _acceleration_enabled = enabled;
        _acceleration_rate = rate;

        if (!_acceleration_enabled)
        {
            _acceleration = 1.0f;
        }
    }

    void FreeCamera::update_vectors()
    {
        const auto rotation = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
        _forward = Vector3::Transform(Vector3::Backward, rotation);
        _up = Vector3::Transform(Vector3::Down, rotation);
        calculate_view_matrix();
    }

    void FreeCamera::update_acceleration(const Vector3& movement, float elapsed)
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
}

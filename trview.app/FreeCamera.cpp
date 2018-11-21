#include "FreeCamera.h"
#include <algorithm>

namespace trview
{
    using namespace DirectX::SimpleMath;

    FreeCamera::FreeCamera(const Size& size)
        : Camera(size)
    {
    }

    float FreeCamera::rotation_yaw() const
    {
        return _rotation_yaw;
    }

    float FreeCamera::rotation_pitch() const
    {
        return _rotation_pitch;
    }

    void FreeCamera::move(const Vector3& movement)
    {
        if (_alignment == Alignment::Camera)
        {
            auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
            _position += Vector3(0, movement.y, 0) + Vector3::Transform(Vector3(movement.x, 0, movement.z), rotate);
        }
        else if (_alignment == Alignment::Axis)
        {
            _position += movement;
        }
        calculate_view_matrix();
    }

    void FreeCamera::set_rotation_pitch(float rotation)
    {
        _rotation_pitch = std::max(-DirectX::XM_PIDIV2, std::min(rotation, DirectX::XM_PIDIV2));
        _forward = Vector3::Transform(Vector3::Backward, Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0));
        calculate_view_matrix();
    }

    void FreeCamera::set_rotation_yaw(float rotation)
    {
        _rotation_yaw = rotation;
        _forward = Vector3::Transform(Vector3::Backward, Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0));
        calculate_view_matrix();
    }

    void FreeCamera::set_position(const Vector3& position)
    {
        _position = position;
        calculate_view_matrix();
    }

    // Set the camera alignment. This controls how the camera movement
    // is applied to the current position.
    // alignment: The new alignment mode.
    void FreeCamera::set_alignment(Alignment alignment)
    {
        _alignment = alignment;
    }
}

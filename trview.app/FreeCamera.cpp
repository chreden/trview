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

    void FreeCamera::set_alignment(Alignment alignment)
    {
        _alignment = alignment;
    }

    void FreeCamera::set_position(const Vector3& position)
    {
        _position = position;
        calculate_view_matrix();
    }

    void FreeCamera::update_vectors()
    {
        _forward = Vector3::Transform(Vector3::Backward, Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0));
        calculate_view_matrix();
    }
}

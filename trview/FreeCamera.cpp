#include "stdafx.h"
#include "FreeCamera.h"

#include <algorithm>

namespace trview
{
    FreeCamera::FreeCamera(const Size& size)
    {
        calculate_view_matrix();
        calculate_projection_matrix(size);
    }

    void FreeCamera::calculate_projection_matrix(const Size& size)
    {
        using namespace DirectX;
        float aspect_ratio = size.width / size.height;
        _projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect_ratio, 0.1f, 10000.0f);
        _view_projection = _view * _projection;
    }

    void FreeCamera::calculate_view_matrix()
    {
        using namespace DirectX;
        using namespace DirectX::SimpleMath;
        auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
        Vector3 up_vector = Vector3::Transform(Vector3::Up, rotate);
        auto target = _position + Vector3::Transform(Vector3(0, 0, 1), rotate);
        _view = XMMatrixLookAtLH(_position, target, up_vector);
        _view_projection = _view * _projection;
    }

    DirectX::SimpleMath::Matrix FreeCamera::view() const
    {
        return _view;
    }

    DirectX::SimpleMath::Matrix FreeCamera::projection() const
    {
        return _projection;
    }

    DirectX::SimpleMath::Matrix FreeCamera::view_projection() const
    {
        return _view_projection;
    }

    DirectX::SimpleMath::Vector3 FreeCamera::target() const
    {
        using namespace DirectX::SimpleMath;
        auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
        return _position + Vector3::Transform(Vector3(0, 0, 1), rotate);
    }

    DirectX::SimpleMath::Vector3 FreeCamera::position() const
    {
        return _position;
    }

    void FreeCamera::move(DirectX::SimpleMath::Vector3 movement)
    {
        using namespace DirectX::SimpleMath;

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
        calculate_view_matrix();
    }

    void FreeCamera::set_rotation_yaw(float rotation)
    {
        _rotation_yaw = rotation;
        calculate_view_matrix();
    }

    float FreeCamera::rotation_pitch() const
    {
        return _rotation_pitch;
    }

    float FreeCamera::rotation_yaw() const
    {
        return _rotation_yaw;
    }
    
    void FreeCamera::set_position(const DirectX::SimpleMath::Vector3& position)
    {
        _position = position;
        calculate_view_matrix();
    }

    DirectX::SimpleMath::Vector3 FreeCamera::up() const
    {
        using namespace DirectX::SimpleMath;
        auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
        return Vector3::Transform(Vector3::Up, rotate);
    }
    
    DirectX::SimpleMath::Vector3 FreeCamera::forward() const
    {
        using namespace DirectX::SimpleMath;
        auto rotate = Matrix::CreateFromYawPitchRoll(_rotation_yaw, _rotation_pitch, 0);
        auto to = Vector3::Transform(Vector3(0, 0, 1), rotate);
        to.Normalize();
        return to;
    }

    // Set the camera alignment. This controls how the camera movement
    // is applied to the current position.
    // alignment: The new alignment mode.
    void FreeCamera::set_alignment(Alignment alignment)
    {
        _alignment = alignment;
    }

    FreeCamera::Alignment camera_mode_to_alignment(CameraMode mode)
    {
        return mode == CameraMode::Axis ? FreeCamera::Alignment::Axis : FreeCamera::Alignment::Camera;
    }

    // Set the dimensions of the render target for the camera.
    // size: The size in pixels of the render target.
    void FreeCamera::set_view_size(const Size& size)
    {
        calculate_projection_matrix(size);
    }
}

#include "stdafx.h"
#include "Camera.h"

#include <algorithm>

namespace trview
{
    Camera::Camera(uint32_t width, uint32_t height)
    {
        // Projection matrix only has to be calculated once, or when the width and height
        // of the window changes.
        calculate_projection_matrix(width, height);
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

    void Camera::calculate_projection_matrix(uint32_t width, uint32_t height)
    {
        using namespace DirectX;
        float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
        _projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect_ratio, 1.0f, 10000.0f);
        _view_projection = _view * _projection;
    }

    void Camera::set_target(const DirectX::XMVECTOR& target)
    {
        _target = target;
        calculate_view_matrix();
    }

    void Camera::calculate_view_matrix()
    {
        using namespace DirectX;
        XMVECTOR eye_position = XMVectorSet(0, 0, -_zoom, 0);
        auto rotate = XMMatrixRotationRollPitchYaw(_rotation_pitch, _rotation_yaw, 0);
        eye_position = XMVector3TransformCoord(eye_position, rotate) + _target;
        XMVECTOR up_vector = XMVectorSet(0, 1, 0, 1);
        _view = XMMatrixLookAtLH(eye_position, _target, up_vector);
        _view_projection = _view * _projection;
    }

    void Camera::set_rotation_pitch(float rotation)
    {
        _rotation_pitch = rotation;
        calculate_view_matrix();
    }

    void Camera::set_rotation_yaw(float rotation)
    {
        _rotation_yaw = rotation;
        calculate_view_matrix();
    }

    void Camera::set_zoom(float zoom)
    {
        _zoom = std::max(zoom, 1.f);
        calculate_view_matrix();
    }

    DirectX::XMMATRIX Camera::view_projection() const
    {
        return _view_projection;
    }
}
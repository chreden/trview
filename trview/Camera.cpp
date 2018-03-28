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

    Camera::Camera(uint32_t width, uint32_t height)
    {
        // Projection matrix only has to be calculated once, or when the width and height
        // of the window changes.
        calculate_projection_matrix(width, height);
    }

    DirectX::XMVECTOR Camera::target() const
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

    void Camera::calculate_projection_matrix(uint32_t width, uint32_t height)
    {
        using namespace DirectX;
        float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
        _projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect_ratio, 0.1f, 10000.0f);
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
        XMVECTOR up_vector = XMVector3TransformCoord(XMVectorSet(0, 1, 0, 1), rotate);
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

    DirectX::XMMATRIX Camera::view() const
    {
        return _view;
    }

    DirectX::XMMATRIX Camera::projection() const
    {
        return _projection;
    }

    DirectX::XMMATRIX Camera::view_projection() const
    {
        return _view_projection;
    }

    void Camera::reset()
    {
        set_rotation_yaw(default_yaw);
        set_rotation_pitch(default_pitch);
        set_zoom(default_zoom);
    }

    DirectX::XMVECTOR Camera::position() const
    {
        using namespace DirectX;
        XMVECTOR eye_position = XMVectorSet(0, 0, -_zoom, 0);
        auto rotate = XMMatrixRotationRollPitchYaw(_rotation_pitch, _rotation_yaw, 0);
        return XMVector3TransformCoord(eye_position, rotate) + _target;
    }
}
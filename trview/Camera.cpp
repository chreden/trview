#include "stdafx.h"
#include "Camera.h"

#include <algorithm>

namespace trview
{
    namespace
    {
        const float max_zoom = 100.0f;
        const float min_zoom = 1.0f;
    }

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

        switch (_mode)
        {
            case Mode::Orbit:
            {
                XMVECTOR eye_position = XMVectorSet(0, 0, -_zoom, 0);
                auto rotate = XMMatrixRotationRollPitchYaw(_rotation_pitch, _rotation_yaw, 0);
                eye_position = XMVector3TransformCoord(eye_position, rotate) + _target;
                XMVECTOR up_vector = XMVector3TransformCoord(XMVectorSet(0, 1, 0, 1), rotate);
                _view = XMMatrixLookAtLH(eye_position, _target, up_vector);
                break;
            }
            case Mode::Free:
            {
                auto rotate = XMMatrixRotationRollPitchYawFromVector(_free_rotation);
                XMVECTOR up_vector = XMVector3TransformCoord(XMVectorSet(0, 1, 0, 1), rotate);
                auto target = XMVectorAdd(_free_position, XMVector3TransformCoord(_free_forward, rotate));
                _view = XMMatrixLookAtLH(_free_position, target, up_vector);
                break;
            }
        }

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

    Camera::Mode Camera::mode() const
    {
        return _mode;
    }

    void Camera::set_mode(Mode mode)
    {
        if (mode == _mode)
        {
            return;
        }
        _mode = mode;

        switch (mode)
        {
            case Mode::Free:
            {
                using namespace DirectX;

                XMVECTOR eye_position = XMVectorSet(0, 0, -_zoom, 0);
                auto rotate = XMMatrixRotationRollPitchYaw(_rotation_pitch, _rotation_yaw, 0);
                eye_position = XMVector3TransformCoord(eye_position, rotate) + _target;

                _free_position = eye_position;
                _free_rotation = XMVectorSet(0, 0, 0, 0);

                _free_forward = XMVector3Normalize(XMVectorSubtract(_target, _free_position));
                _free_up = XMVector3TransformCoord(XMVectorSet(0, 1, 0, 0), rotate);
                _free_right = XMVector3Cross(_free_forward, _free_up);
                break;
            }
            case Mode::Orbit:
            {
                reset();
                break;
            }
        }
    }

    void Camera::move(DirectX::XMVECTOR movement)
    {
        using namespace DirectX;

        XMFLOAT3 move;
        XMStoreFloat3(&move, movement);
        auto forward = XMVectorScale(_free_forward, move.z);
        auto right = XMVectorScale(_free_right, move.x);
        _free_position = XMVectorAdd(_free_position, XMVectorAdd(forward, right));
    }
}
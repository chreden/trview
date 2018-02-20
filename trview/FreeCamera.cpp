#include "stdafx.h"
#include "FreeCamera.h"

#include <algorithm>

namespace trview
{
    FreeCamera::FreeCamera(uint32_t width, uint32_t height)
        : _free_forward(DirectX::XMVectorSet(0,0,1,0)),
        _free_up(DirectX::XMVectorSet(0,1,0,0)),
        _free_right(DirectX::XMVectorSet(1,0,0,0)),
        _free_position(DirectX::XMVectorSet(0,0,0,0))
    {
        // Projection matrix only has to be calculated once, or when the width and height
        // of the window changes.
        calculate_view_matrix();
        calculate_projection_matrix(width, height);

        /*
            XMVECTOR eye_position = XMVectorSet(0, 0, -_zoom, 0);
            auto rotate = XMMatrixRotationRollPitchYaw(_rotation_pitch, _rotation_yaw, 0);
            eye_position = XMVector3TransformCoord(eye_position, rotate) + _target;

            _free_position = eye_position;
            _rotation_yaw = 0;
            _rotation_pitch = 0;

            _free_forward = XMVector3Normalize(XMVectorSubtract(_target, _free_position));
            _free_up = XMVector3TransformCoord(XMVectorSet(0, 1, 0, 0), rotate);
            _free_right = XMVector3Normalize(XMVector3Cross(_free_forward, _free_up));
        */
    }

    void FreeCamera::calculate_projection_matrix(uint32_t width, uint32_t height)
    {
        using namespace DirectX;
        float aspect_ratio = static_cast<float>(width) / static_cast<float>(height);
        _projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect_ratio, 1.0f, 10000.0f);
        _view_projection = _view * _projection;
    }

    void FreeCamera::calculate_view_matrix()
    {
        using namespace DirectX;
        auto rotate = XMMatrixRotationRollPitchYaw(_rotation_pitch, _rotation_yaw, 0);
        XMVECTOR up_vector = XMVector3TransformCoord(_free_up, rotate);
        auto target = XMVectorAdd(_free_position, XMVector3TransformCoord(_free_forward, rotate));
        _view = XMMatrixLookAtLH(_free_position, target, up_vector);
        _view_projection = _view * _projection;
    }

    DirectX::XMMATRIX FreeCamera::view_projection() const
    {
        return _view_projection;
    }

    void FreeCamera::move(DirectX::XMVECTOR movement)
    {
        using namespace DirectX;

        auto rotate = XMMatrixRotationRollPitchYaw(_rotation_pitch, _rotation_yaw, 0);
        auto free_forward = XMVector3TransformCoord(_free_forward, rotate);
        auto free_right = XMVector3TransformCoord(_free_right, rotate);

        XMFLOAT3 move;
        XMStoreFloat3(&move, movement);
        auto forward = XMVectorScale(free_forward, move.z);
        auto right = XMVectorScale(free_right, move.x);
        _free_position = XMVectorAdd(_free_position, XMVectorAdd(forward, right));
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
    
    void FreeCamera::set_position(DirectX::XMVECTOR position)
    {
        _free_position = position;
        calculate_view_matrix();
    }
}

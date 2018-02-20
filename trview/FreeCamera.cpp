#include "stdafx.h"
#include "FreeCamera.h"

#include <algorithm>

namespace trview
{
    FreeCamera::FreeCamera(uint32_t width, uint32_t height)
        : _position(DirectX::XMVectorSet(0,0,0,0))
    {
        // Projection matrix only has to be calculated once, or when the width and height
        // of the window changes.
        calculate_view_matrix();
        calculate_projection_matrix(width, height);
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
        XMVECTOR up_vector = XMVector3TransformCoord(XMVectorSet(0,1,0,0), rotate);
        auto target = XMVectorAdd(_position, XMVector3TransformCoord(XMVectorSet(0,0,1,0), rotate));
        _view = XMMatrixLookAtLH(_position, target, up_vector);
        _view_projection = _view * _projection;
    }

    DirectX::XMMATRIX FreeCamera::view_projection() const
    {
        return _view_projection;
    }

    void FreeCamera::move(DirectX::XMVECTOR movement)
    {
        using namespace DirectX;

        XMFLOAT3 xyz;
        XMStoreFloat3(&xyz, movement);
        
        auto rotate = XMMatrixRotationRollPitchYaw(_rotation_pitch, _rotation_yaw, 0);
        _position = 
            XMVectorAdd(_position, 
                XMVectorAdd(XMVectorSet(0, xyz.y, 0, 0), 
                    XMVector3TransformCoord(XMVectorSet(xyz.x, 0, xyz.z, 0), rotate)));

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
        _position = position;
        calculate_view_matrix();
    }
}

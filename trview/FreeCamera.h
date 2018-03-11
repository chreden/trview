#pragma once

#include <cstdint>
#include <DirectXMath.h>

namespace trview
{
    class FreeCamera
    {
    public:
        FreeCamera(uint32_t width, uint32_t height);
        void                move(DirectX::XMVECTOR movement);
        DirectX::XMMATRIX   view() const;
        DirectX::XMMATRIX   projection() const;
        DirectX::XMMATRIX   view_projection() const;

        DirectX::XMVECTOR   target() const;
        DirectX::XMVECTOR   position() const;
        float               rotation_yaw() const;
        float               rotation_pitch() const;
        void                set_rotation_yaw(float rotation);
        void                set_rotation_pitch(float rotation);
        void                set_position(DirectX::XMVECTOR position);
    private:
        void calculate_projection_matrix(uint32_t width, uint32_t height);
        void calculate_view_matrix();

        DirectX::XMMATRIX _view;
        DirectX::XMMATRIX _projection;
        DirectX::XMMATRIX _view_projection;

        DirectX::XMVECTOR _position;

        float _rotation_yaw{ 0.0f };
        float _rotation_pitch{ 0.0f };
    };
}
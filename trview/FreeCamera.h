#pragma once

#include <cstdint>
#include <DirectXMath.h>

#include "ICamera.h"

namespace trview
{
    class FreeCamera : public ICamera
    {
    public:
        FreeCamera(uint32_t width, uint32_t height);
        virtual ~FreeCamera() = default;
        void                move(DirectX::XMVECTOR movement);
        virtual DirectX::XMMATRIX   view() const override;
        virtual DirectX::XMMATRIX   projection() const override;
        virtual DirectX::XMMATRIX   view_projection() const override;

        DirectX::XMVECTOR   target() const;
        virtual DirectX::XMVECTOR   position() const override;
        virtual DirectX::XMVECTOR up() const override;
        virtual DirectX::XMVECTOR forward() const override;
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
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
        virtual DirectX::SimpleMath::Matrix   view() const override;
        virtual DirectX::SimpleMath::Matrix   projection() const override;
        virtual DirectX::SimpleMath::Matrix   view_projection() const override;

        DirectX::SimpleMath::Vector3   target() const;
        virtual DirectX::SimpleMath::Vector3   position() const override;
        virtual DirectX::SimpleMath::Vector3 up() const override;
        virtual DirectX::SimpleMath::Vector3 forward() const override;
        float               rotation_yaw() const;
        float               rotation_pitch() const;
        void                set_rotation_yaw(float rotation);
        void                set_rotation_pitch(float rotation);
        void                set_position(DirectX::XMVECTOR position);
    private:
        void calculate_projection_matrix(uint32_t width, uint32_t height);
        void calculate_view_matrix();

        DirectX::SimpleMath::Matrix _view;
        DirectX::SimpleMath::Matrix _projection;
        DirectX::SimpleMath::Matrix _view_projection;

        DirectX::SimpleMath::Vector3 _position;

        float _rotation_yaw{ 0.0f };
        float _rotation_pitch{ 0.0f };
    };
}
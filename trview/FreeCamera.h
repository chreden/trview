#pragma once

#include <cstdint>
#include "ICamera.h"
#include "CameraMode.h"

namespace trview
{
    class FreeCamera : public ICamera
    {
    public:
        enum class Alignment
        {
            Camera,
            Axis
        };

        FreeCamera(uint32_t width, uint32_t height);
        virtual ~FreeCamera() = default;
        void move(DirectX::SimpleMath::Vector3 movement);
        virtual DirectX::SimpleMath::Matrix   view() const override;
        virtual DirectX::SimpleMath::Matrix   projection() const override;
        virtual DirectX::SimpleMath::Matrix   view_projection() const override;

        DirectX::SimpleMath::Vector3   target() const;
        virtual DirectX::SimpleMath::Vector3   position() const override;
        virtual DirectX::SimpleMath::Vector3 up() const override;
        virtual DirectX::SimpleMath::Vector3 forward() const override;
        float rotation_yaw() const override;
        float rotation_pitch() const override;
        void set_rotation_yaw(float rotation) override;
        void set_rotation_pitch(float rotation) override;
        void set_position(const DirectX::SimpleMath::Vector3& position);

        // Set the camera alignment. This controls how the camera movement
        // is applied to the current position.
        // alignment: The new alignment mode.
        void set_alignment(Alignment alignment);
    private:
        void calculate_projection_matrix(uint32_t width, uint32_t height);
        void calculate_view_matrix();

        DirectX::SimpleMath::Matrix _view;
        DirectX::SimpleMath::Matrix _projection;
        DirectX::SimpleMath::Matrix _view_projection;

        DirectX::SimpleMath::Vector3 _position;
        Alignment _alignment{ Alignment::Axis };

        float _rotation_yaw{ 0.0f };
        float _rotation_pitch{ 0.0f };
    };

    FreeCamera::Alignment camera_mode_to_alignment(CameraMode mode);
}
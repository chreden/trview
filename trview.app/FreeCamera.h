#pragma once

#include <trview.common/Size.h>
#include "Camera.h"

namespace trview
{
    class FreeCamera : public Camera
    {
    public:
        enum class Alignment
        {
            Camera,
            Axis
        };

        explicit FreeCamera(const Size& size);
        virtual ~FreeCamera() = default;
        void move(const DirectX::SimpleMath::Vector3& movement);

        virtual float rotation_yaw() const override;
        virtual float rotation_pitch() const override;

        virtual void set_rotation_yaw(float rotation) override;
        virtual void set_rotation_pitch(float rotation) override;
        void set_position(const DirectX::SimpleMath::Vector3& position);

        // Set the camera alignment. This controls how the camera movement
        // is applied to the current position.
        // alignment: The new alignment mode.
        void set_alignment(Alignment alignment);
    private:
        const float default_pitch = -0.78539f;
        const float default_yaw = 0.0f;

        Alignment _alignment{ Alignment::Axis };
        float _rotation_yaw{ 0.0f };
        float _rotation_pitch{ 0.0f };
    };
}
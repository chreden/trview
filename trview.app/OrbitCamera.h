#pragma once

#include <trview.common/Size.h>

#include "Camera.h"

namespace trview
{
    class OrbitCamera : public Camera
    {
    public:
        explicit OrbitCamera(const Size& size);
        virtual ~OrbitCamera() = default;
        float rotation_yaw() const override;
        float rotation_pitch() const override;
        float               zoom() const;
        DirectX::SimpleMath::Vector3   target() const;
        void                set_target(const DirectX::SimpleMath::Vector3& target);
        void                set_rotation_yaw(float rotation) override;
        void                set_rotation_pitch(float rotation) override;
        void                set_zoom(float zoom);
        void                reset();
    protected:
        const float default_pitch = -0.78539f;
        const float default_yaw = 0.0f;
        const float default_zoom = 8.0f;

        // This is the orbit target.
        DirectX::SimpleMath::Vector3 _target;
        DirectX::SimpleMath::Matrix _view;
        DirectX::SimpleMath::Matrix _view_lh;
        DirectX::SimpleMath::Matrix _projection;
        DirectX::SimpleMath::Matrix _projection_lh;
        DirectX::SimpleMath::Matrix _view_projection;
        float             _rotation_yaw{ default_yaw };
        float             _rotation_pitch{ default_pitch };
        float             _zoom{ default_zoom };
    };
}

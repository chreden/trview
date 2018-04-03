#pragma once

#include "ICamera.h"

namespace trview
{
    class Camera : public ICamera
    {
    public:
        Camera(uint32_t width, uint32_t height);
        virtual ~Camera() = default;
        float               rotation_yaw() const;
        float               rotation_pitch() const;
        float               zoom() const;
        DirectX::SimpleMath::Vector3   target() const;
        void                set_target(const DirectX::SimpleMath::Vector3& target);
        virtual DirectX::SimpleMath::Matrix   view() const override;
        virtual DirectX::SimpleMath::Matrix   projection() const override;
        virtual DirectX::SimpleMath::Matrix   view_projection() const override;
        void                set_rotation_yaw(float rotation);
        void                set_rotation_pitch(float rotation);
        void                set_zoom(float zoom);
        void                reset();
        virtual DirectX::SimpleMath::Vector3   position() const override;
        virtual DirectX::SimpleMath::Vector3 up() const override;
        virtual DirectX::SimpleMath::Vector3 forward() const override;
    private:
        void calculate_projection_matrix(uint32_t width, uint32_t height);
        void calculate_view_matrix();

        const float default_pitch = 0.78539f;
        const float default_yaw = 0.0f;
        const float default_zoom = 8.0f;

        // This is the orbit target.
        DirectX::SimpleMath::Vector3 _target;
        DirectX::SimpleMath::Matrix _view;
        DirectX::SimpleMath::Matrix _projection;
        DirectX::SimpleMath::Matrix _view_projection;
        float             _rotation_yaw{ default_yaw };
        float             _rotation_pitch{ default_pitch };
        float             _zoom{ default_zoom };
    };
}

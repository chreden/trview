#pragma once

#include <DirectXMath.h>

namespace trview
{
    class Camera
    {
    public:
        // Defines the behaviour of the camera.
        enum class Mode
        {
            // The camera is orbiting around the centre of a room.
            Orbit,
            // The camera is free roaming - the user is in control.
            Free
        };

                            Camera(uint32_t width, uint32_t height);
        float               rotation_yaw() const;
        float               rotation_pitch() const;
        float               zoom() const;
        void                set_target(const DirectX::XMVECTOR& target);
        DirectX::XMMATRIX   view_projection() const;
        void                set_rotation_yaw(float rotation);
        void                set_rotation_pitch(float rotation);
        void                set_zoom(float zoom);
        void                reset();
        Mode                mode() const;
        void                set_mode(Mode mode);
    private:
        void calculate_projection_matrix(uint32_t width, uint32_t height);
        void calculate_view_matrix();

        const float default_pitch = 0.78539f;
        const float default_yaw = 0.0f;
        const float default_zoom = 8.0f;

        DirectX::XMVECTOR _target;
        DirectX::XMMATRIX _view;
        DirectX::XMMATRIX _projection;
        DirectX::XMMATRIX _view_projection;
        float             _rotation_yaw{ default_yaw };
        float             _rotation_pitch{ default_pitch };
        float             _zoom{ default_zoom };
        Mode              _mode{ Mode::Orbit };
    };
}

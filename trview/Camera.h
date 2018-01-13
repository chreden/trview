#pragma once

#include <DirectXMath.h>

namespace trview
{
    class Camera
    {
    public:
                            Camera(uint32_t width, uint32_t height);
        float               rotation_yaw() const;
        float               rotation_pitch() const;
        float               zoom() const;
        void                set_target(const DirectX::XMVECTOR& target);
        DirectX::XMMATRIX   view_projection() const;
        void                set_rotation_yaw(float rotation);
        void                set_rotation_pitch(float rotation);
        void                set_zoom(float zoom);
    private:
        void calculate_projection_matrix(uint32_t width, uint32_t height);
        void calculate_view_matrix();

        DirectX::XMVECTOR _target;
        DirectX::XMMATRIX _view;
        DirectX::XMMATRIX _projection;
        DirectX::XMMATRIX _view_projection;
        float             _rotation_yaw{ 0.f };
        float             _rotation_pitch{ 0.78539f };
        float             _zoom{ 8.f };
    };
}

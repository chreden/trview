#pragma once

#include "ICamera.h"

namespace trview
{
    class Camera : public ICamera
    {
    public:
        explicit Camera(const Size& size);
        virtual ~Camera() = default;
        virtual Size view_size() const override;
        virtual const DirectX::SimpleMath::Matrix& view() const override;
        virtual const DirectX::SimpleMath::Matrix& projection() const override;
        virtual const DirectX::SimpleMath::Matrix& view_projection() const override;
        virtual void set_view_size(const Size& size) override;
        virtual const DirectX::BoundingFrustum& frustum() const override;
        virtual DirectX::SimpleMath::Vector3 position() const override;
        virtual DirectX::SimpleMath::Vector3 up() const override;
        virtual DirectX::SimpleMath::Vector3 forward() const override;
    protected:
        void calculate_view_matrix();
        void calculate_projection_matrix();
        void calculate_bounding_frustum();
        DirectX::SimpleMath::Vector3 _position;
        DirectX::SimpleMath::Vector3 _forward;
        DirectX::SimpleMath::Vector3 _up;
    private:
        Size _view_size;
        DirectX::SimpleMath::Matrix _view;
        DirectX::SimpleMath::Matrix _view_lh;
        DirectX::SimpleMath::Matrix _projection;
        DirectX::SimpleMath::Matrix _projection_lh;
        DirectX::SimpleMath::Matrix _view_projection;
        DirectX::BoundingFrustum _bounding_frustum;
    };
}

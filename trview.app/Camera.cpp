#include "Camera.h"
#include <algorithm>

namespace trview
{
    using namespace DirectX::SimpleMath;

    Camera::Camera(const Size& size)
        : _view_size(size), _forward(Vector3::Forward), _up(Vector3::Down), _position(Vector3::Zero)
    {
        calculate_view_matrix();
        calculate_projection_matrix();
    }

    void Camera::set_view_size(const Size& size)
    {
        _view_size = size;
        calculate_projection_matrix();
    }

    const Matrix& Camera::view() const
    {
        return _view;
    }

    const Matrix& Camera::projection() const
    {
        return _projection;
    }

    const Matrix& Camera::view_projection() const
    {
        return _view_projection;
    }

    Size Camera::view_size() const
    {
        return _view_size;
    }

    void Camera::calculate_view_matrix()
    {
        _view = XMMatrixLookAtRH(_position, _position + _forward, _up);
        _view_lh = XMMatrixLookAtLH(_position, _position + _forward, _up);
        _view_projection = _view * _projection;
        calculate_bounding_frustum();
    }

    void Camera::calculate_projection_matrix()
    {
        using namespace DirectX;
        float aspect_ratio = _view_size.width / _view_size.height;
        _projection = XMMatrixPerspectiveFovRH(XM_PIDIV4, aspect_ratio, 0.1f, 10000.0f);
        _projection_lh = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspect_ratio, 0.1f, 10000.0f);
        _view_projection = _view * _projection;
        calculate_bounding_frustum();
    }

    void Camera::calculate_bounding_frustum()
    {
        using namespace DirectX;
        XMVECTOR determinant;
        XMMATRIX inv_view_lh = XMMatrixInverse(&determinant, _view_lh);
        BoundingFrustum boundingFrustum(_projection_lh);
        boundingFrustum.Transform(_bounding_frustum, inv_view_lh);
    }

    const DirectX::BoundingFrustum& Camera::frustum() const
    {
        return _bounding_frustum;
    }

    Vector3 Camera::position() const
    {
        return _position;
    }
    
    Vector3 Camera::up() const
    {
        return _up;
    }

    Vector3 Camera::forward() const
    {
        return _forward;
    }

    void Camera::set_rotation_pitch(float rotation)
    {
        _rotation_pitch = std::max(-DirectX::XM_PIDIV2, std::min(rotation, DirectX::XM_PIDIV2));
        update_vectors();
    }

    void Camera::set_rotation_yaw(float rotation)
    {
        _rotation_yaw = rotation;
        update_vectors();
    }

    float Camera::rotation_pitch() const
    {
        return _rotation_pitch;
    }

    float Camera::rotation_yaw() const
    {
        return _rotation_yaw;
    }
}

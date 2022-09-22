#pragma once

#include "../../Camera/ICamera.h"

namespace trview
{
    namespace mocks
    {
        struct MockCamera : public ICamera
        {
            virtual ~MockCamera() = default;
            MOCK_METHOD(DirectX::SimpleMath::Vector3, forward, (), (const, override));
            MOCK_METHOD(const DirectX::BoundingFrustum, frustum, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, position, (), (const, override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, rendering_position, (), (const, override));
            MOCK_METHOD(const DirectX::SimpleMath::Matrix, projection, (), (const, override));
            MOCK_METHOD(ProjectionMode, projection_mode, (), (const, override));
            MOCK_METHOD(float, rotation_pitch, (), (const, override));
            MOCK_METHOD(float, rotation_yaw, (), (const, override));
            MOCK_METHOD(void, rotate_to_pitch, (float), (override));
            MOCK_METHOD(void, rotate_to_yaw, (float), (override));
            MOCK_METHOD(void, set_projection_mode, (ProjectionMode), (override));
            MOCK_METHOD(void, set_rotation_pitch, (float), (override));
            MOCK_METHOD(void, set_rotation_yaw, (float), (override));
            MOCK_METHOD(void, set_view_size, (const Size&), (override));
            MOCK_METHOD(void, set_zoom, (float), (override));
            MOCK_METHOD(DirectX::SimpleMath::Vector3, up, (), (const, override));
            MOCK_METHOD(void, update, (float), (override));
            MOCK_METHOD(const DirectX::SimpleMath::Matrix, view, (), (const, override));
            MOCK_METHOD(const DirectX::SimpleMath::Matrix, view_projection, (), (const, override));
            MOCK_METHOD(const Size, view_size, (), (const, override));
            MOCK_METHOD(float, zoom, (), (const, override));
            MOCK_METHOD(bool, idle_rotation, (), (const, override));
            MOCK_METHOD(void, set_fov, (float), (override));
        };
    }
}

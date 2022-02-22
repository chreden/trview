#pragma once

#include "../../UI/ICameraControls.h"

namespace trview
{
    namespace mocks
    {
        struct MockCameraControls final : public ICameraControls
        {
            virtual ~MockCameraControls() = default;
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_mode, (CameraMode), (override));
            MOCK_METHOD(void, set_projection_mode, (ProjectionMode), (override));
        };
    }
}

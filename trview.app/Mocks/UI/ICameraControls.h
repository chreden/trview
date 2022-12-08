#pragma once

#include "../../UI/ICameraControls.h"

namespace trview
{
    namespace mocks
    {
        struct MockCameraControls : public ICameraControls
        {
            MockCameraControls();
            virtual ~MockCameraControls();
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_mode, (CameraMode), (override));
            MOCK_METHOD(void, set_projection_mode, (ProjectionMode), (override));
        };
    }
}

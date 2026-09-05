module;

#include <gmock/gmock.h>

export module trview.app.mocks:MockCameraControls;

import trview.app;

namespace trview
{
    namespace mocks
    {
        export struct MockCameraControls : public ICameraControls
        {
            MOCK_METHOD(void, render, (), (override));
            MOCK_METHOD(void, set_mode, (ICamera::Mode), (override));
            MOCK_METHOD(void, set_projection_mode, (ProjectionMode), (override));
        };
    }
}

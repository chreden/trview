#pragma once

#include "../../Geometry/IPicking.h"

namespace trview
{
    namespace mocks
    {
        struct MockPicking : public IPicking
        {
            MockPicking();
            virtual ~MockPicking();
            MOCK_METHOD(void, pick, (const ICamera&), (override));
        };
    }
}

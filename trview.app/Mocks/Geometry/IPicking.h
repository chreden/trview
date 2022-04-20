#pragma once

#include "../../Geometry/IPicking.h"

namespace trview
{
    namespace mocks
    {
        struct MockPicking : public IPicking
        {
            virtual ~MockPicking() = default;
            MOCK_METHOD(void, pick, (const ICamera&), (override));
        };
    }
}

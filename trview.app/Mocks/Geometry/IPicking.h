#pragma once

#include "../../Geometry/IPicking.h"

namespace trview
{
    namespace mocks
    {
        class MockPicking final : public IPicking
        {
        public:
            MOCK_METHOD(void, pick, (const Window&, const ICamera&));
        };
    }
}

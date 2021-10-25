#pragma once

#include <trview.app/Tools/IMover.h>

namespace trview
{
    namespace mocks
    {
        struct MockMover final : public IMover
        {
            virtual ~MockMover() = default;
            MOCK_METHOD(void, render, (const ICamera&), (override));
        };
    }
}
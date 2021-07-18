#pragma once

#include <trview.app/UI/IBubble.h>

namespace trview
{
    namespace mocks
    {
        struct MockBubble final : public IBubble
        {
            virtual ~MockBubble() = default;
            MOCK_METHOD(void, show, (const Point&), (override));
        };
    }
}

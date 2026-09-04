module;

#include <gmock/gmock.h>
#include <SimpleMath.h>

export module trview.app:MockSectorHighlight;

import :ISectorHighlight;

namespace trview
{
    namespace mocks
    {
        export struct MockSectorHighlight : public ISectorHighlight
        {
            MOCK_METHOD(void, set_sector, (const std::shared_ptr<ISector>&, const DirectX::SimpleMath::Matrix&), (override));
            MOCK_METHOD(void, render, (const ICamera&), (override));
        };
    }
}

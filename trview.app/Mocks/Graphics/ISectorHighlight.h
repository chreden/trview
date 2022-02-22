#pragma once

#include "../../Graphics/ISectorHighlight.h"

namespace trview
{
    namespace mocks
    {
        struct MockSectorHighlight : public ISectorHighlight
        {
            virtual ~MockSectorHighlight() = default;
            MOCK_METHOD(void, set_sector, (const std::shared_ptr<ISector>&, const DirectX::SimpleMath::Matrix&), (override));
            MOCK_METHOD(void, render, (const ICamera&, const ILevelTextureStorage&), (override));
        };
    }
}

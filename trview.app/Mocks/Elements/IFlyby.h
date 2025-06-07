#pragma once

#include "../../Elements/Flyby/IFlyby.h"

namespace trview
{
    namespace mocks
    {
        struct MockFlyby : public IFlyby
        {
            explicit MockFlyby();
            virtual ~MockFlyby();
            MOCK_METHOD(void, get_transparent_triangles, (ITransparencyBuffer&, const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, render, (const ICamera&, const DirectX::SimpleMath::Color&), (override));
            MOCK_METHOD(void, set_visible, (bool), (override));
            MOCK_METHOD(bool, visible, (), (const, override));
        };
    }
}
